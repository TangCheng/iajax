#include <gio/gio.h>
#include <string.h>
#include "ajax.h"
#include "iajax.h"
#include "http_parser.h"
#include "http_request.h"
#include "http_proc.h"
#include "http_response.h"
#include "common.h"
#include "login/sha1.h"

enum {
  PROP_0,
  PROP_APP,
  PROP_ADDRESS,
  PROP_PORT,
  N_PROPERTIES
};

typedef struct _IPcamAjaxPrivate
{
    IpcamIAjax *iajax;
    gchar *address;
    guint port;
    GThread *thread;
    volatile gboolean terminated;
} IpcamAjaxPrivate;

typedef struct _IpcamAjaxWorkerData
{
    IpcamIAjax *iajax;
    GSocket *socket;
} IpcamAjaxWorkerData;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamAjax, ipcam_ajax, G_TYPE_OBJECT)

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static gpointer ajax_worker(gpointer data);
static gpointer request_proc(gpointer data);

static void ipcam_ajax_finalize(GObject *object)
{
    IpcamAjax *ajax = IPCAM_AJAX(object);
    IpcamAjaxPrivate *priv = ipcam_ajax_get_instance_private(ajax);
    priv->terminated = TRUE;
    g_thread_join(priv->thread);
    g_clear_pointer(&priv->thread, g_thread_unref);
    g_free(priv->address);
    G_OBJECT_CLASS(ipcam_ajax_parent_class)->finalize(object);
}

static GObject *ipcam_ajax_constructor(GType gtype,
                                       guint  n_properties,
                                       GObjectConstructParam *properties)
{
    GObject *obj;
    IpcamAjax *ajax;
    IpcamAjaxPrivate *priv;

    /* Always chain up to the parent constructor */
    obj = G_OBJECT_CLASS(ipcam_ajax_parent_class)->constructor(gtype, n_properties, properties);

    ajax = IPCAM_AJAX(obj);
    g_assert(IPCAM_IS_AJAX(ajax));

    priv = ipcam_ajax_get_instance_private(ajax);

    /* thread must be create after construction has alread initialized the properties */
    priv->terminated = FALSE;
    priv->thread = g_thread_new("ajax-worker", ajax_worker, ajax);

    return obj;
}

static void ipcam_ajax_init(IpcamAjax *ajax)
{
}

static void ipcam_ajax_get_property(GObject    *object,
                                    guint       property_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
    IpcamAjax *self = IPCAM_AJAX(object);
    IpcamAjaxPrivate *priv = ipcam_ajax_get_instance_private(self);
    switch(property_id)
    {
    case PROP_APP:
        {
            g_value_set_object(value, priv->iajax);
        }
        break;
    case PROP_ADDRESS:
        {
            g_value_set_string(value, priv->address);
        }
        break;
    case PROP_PORT:
        {
            g_value_set_int(value, priv->port);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_ajax_set_property(GObject      *object,
                                    guint         property_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
    IpcamAjax *self = IPCAM_AJAX(object);
    IpcamAjaxPrivate *priv = ipcam_ajax_get_instance_private(self);
    switch(property_id)
    {
    case PROP_APP:
        {
            priv->iajax = g_value_get_object(value);
        }
        break;
    case PROP_ADDRESS:
        {
            g_free(priv->address);
            priv->address = g_value_dup_string(value);
        }
        break;
    case PROP_PORT:
        {
            priv->port = g_value_get_int(value);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_ajax_class_init(IpcamAjaxClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->constructor = ipcam_ajax_constructor;
    object_class->finalize = &ipcam_ajax_finalize;
    object_class->get_property = &ipcam_ajax_get_property;
    object_class->set_property = &ipcam_ajax_set_property;

    obj_properties[PROP_APP] =
        g_param_spec_object("app",
                            "Application",
                            "The parent application of the ajax thread.",
                            IPCAM_IAJAX_TYPE, // default value
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    obj_properties[PROP_ADDRESS] =
        g_param_spec_string("address",
                            "Web service listen address.",
                            "Web service listen address.",
                            NULL, // default value
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
    obj_properties[PROP_PORT] =
        g_param_spec_int("port",
                         "Web service listen port.",
                         "Web service listen port.",
                         1024,
                         65535,
                         1025, // default value
                         G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
gboolean ipcam_ajax_get_terminated(IpcamAjax *ajax)
{
    g_return_val_if_fail(IPCAM_IS_AJAX(ajax), FALSE);
    IpcamAjaxPrivate *priv = ipcam_ajax_get_instance_private(ajax);
    return priv->terminated;
}
static gpointer ajax_worker(gpointer data)
{
    IpcamAjax *ajax = data;
    gchar *address;
    guint port;
    g_object_get(ajax, "address", &address, "port", &port, NULL);
    GInetAddress *inet_address = g_inet_address_new_from_string(address);
    GSocketAddress *socket_address = g_inet_socket_address_new(inet_address, port);
    //g_free(inet_address);
    g_free(address);
    GSocket *server = g_socket_new(G_SOCKET_FAMILY_IPV4,
                                   G_SOCKET_TYPE_STREAM,
                                   G_SOCKET_PROTOCOL_TCP,
                                   NULL);

    g_socket_set_blocking(server, TRUE);

    g_assert(g_socket_bind(server, socket_address, TRUE, NULL));
    g_assert(g_socket_listen(server, NULL));

    while (!ipcam_ajax_get_terminated(ajax))
    {
        GSocket *worker = g_socket_accept(server, NULL, NULL);
        if (worker)
        {
            g_socket_set_blocking(worker, TRUE);
            IpcamAjaxWorkerData *data = g_new(IpcamAjaxWorkerData, 1);
            g_object_get(ajax, "app", &data->iajax, NULL);
            data->socket = worker;

            GThread *thread =g_thread_new("request-proc", request_proc, data);
            g_thread_unref(thread);
        }
        else
        {
            g_warn_if_reached();
            sleep(1);
        }
    }

    g_socket_close(server, NULL);
    g_object_unref(server);

    return NULL;
}
static gboolean check_authorize(IpcamIAjax *iajax, IpcamHttpRequest *request, gchar **role)
{
    gboolean ret = FALSE;
    gchar *path = NULL;

    g_object_get(request, "path", &path, NULL);
    if (path)
    {
        if (g_strstr_len(path, -1, "login") ||
            g_strstr_len(path, -1, "upgrade"))
        {
            ret = TRUE;
        }
        else
        {
            gchar *cookie = ipcam_http_request_get_header_value(request, "Set-Cookie");
            if (cookie)
            {
                gchar *token_start = g_strstr_len(cookie, -1, "token=");
                if (token_start)
                {
                    gchar *token = g_malloc0(SHA1_DIGEST_SIZE * 2 + 1);
                    guint i = 0;
                    if (token)
                    {
                        do 
                        {
                            token[i] = token_start[6 + i];
                            i++;
                        } while (!(token_start[6 + i] == '\0' || token_start[6 + i] == ';'));
                    
                        ret = ipcam_iajax_check_login(iajax, token, role);
                        g_free(token);
                    }
                }
                g_free(cookie);
            }
        }
        g_free(path);
    }

    return ret;
}
gboolean check_operators_permission(const gchar *path)
{
    g_return_val_if_fail(path, FALSE);
    gboolean ret = FALSE;
    gint i = 0;
    const gchar *paths[] =
    {
        "login.json",
        "logout.json",
        "upgrade.json",
        "misc.json",
        "vidoe.json",
        "network.json",
        "day_night_mode.json",
        "system.json",
        "event.json",
        "ptz.json"
    };
    
    for (i = 0; i < ARRAY_SIZE(paths); i++)
    {
        if (g_str_equal(path, paths[i]))
        {
            ret = TRUE;
            break;
        }   
    }

    return ret;
}
gboolean check_users_permission(const gchar *path)
{
    g_return_val_if_fail(path, FALSE);
    gboolean ret = FALSE;
    gint i = 0;
    const gchar *paths[] =
    {
        "login.json",
        "logout.json",
        "upgrade.json",
        "logout.json",
        "misc.json",
        "vidoe.json",
        "network.json"
    };

    for (i = 0; i < ARRAY_SIZE(paths); i++)
    {
        if (g_str_equal(path, paths[i]))
        {
            ret = TRUE;
            break;
        }   
    }
 
    return ret;
}
static gboolean check_permission(IpcamIAjax *iajax, IpcamHttpRequest *request, gchar *role)
{
    gboolean ret = FALSE;
    gchar *path = NULL;

    g_object_get(request, "path", &path, NULL);
    if (path)
    {
        if (role)
        {
            if (g_str_equal(role, "administrator"))
            {
                ret = TRUE;
            }
            else if (g_str_equal(role, "operator"))
            {
                ret = check_operators_permission(g_strrstr(path, "/") + 1);
            }
            else
            {
                ret = check_users_permission(g_strrstr(path, "/") + 1);
            }
        }
        else
        {
            ret = check_users_permission(g_strrstr(path, "/") + 1);
        }
        g_free(path);
    }
    
    return ret;
}
static gboolean check_send_response(IpcamHttpRequest *request, IpcamHttpResponse *response)
{
    gboolean ret = FALSE;
    gchar *path = NULL;

    g_object_get(request, "path", &path, NULL);
    if (path)
    {
        if (g_strstr_len(path, -1, "login"))
        {
            ret = TRUE;
        }
        else
        {
            guint code;
            g_object_get(response, "status", &code, NULL);
            if (ipcam_http_request_is_get(request))
            {
                ret = TRUE;
            }
            else if (code == 400)
            {
                ret = TRUE;
            }
        }
        g_free(path);
    }

    return ret;
}
static gpointer request_proc(gpointer data)
{
#define BUFFER_SIZE 2048
    IpcamAjaxWorkerData *params = data;
    IpcamIAjax *app = params->iajax;
    GSocket *worker = params->socket;
    gchar *buffer = g_new0(gchar, BUFFER_SIZE);
    gssize len;
    gboolean send_response = TRUE;
    gchar *role = NULL;

    len = g_socket_receive(worker, buffer, BUFFER_SIZE, NULL, NULL);
    if (len > 0)
    {
        IpcamHttpParser *parser = g_object_new(IPCAM_HTTP_PARSER_TYPE, NULL);
        IpcamHttpRequest *request = ipcam_http_parser_get_request(parser, buffer, len);
        IpcamHttpResponse *response = NULL;
        
        if (check_authorize(app, request, &role))
        {
            if (check_permission(app, request, role))
            {
                IpcamHttpProc *proc = g_object_new(IPCAM_HTTP_PROC_TYPE, "app", app, NULL);
                response = ipcam_http_proc_get_response(proc, request, worker);
                send_response = check_send_response(request, response);
                g_clear_object(&proc);
            }
            else
            {
                response = ipcam_http_request_get_response(request, 403);
            }
        }
        else
        {
            response = ipcam_http_request_get_response(request, 401);
        }

        if (send_response && response)
        {
            ipcam_http_response_write_string(response, worker);
            g_clear_object(&response);
        }
        g_free(role);
        g_clear_object(&request);
        g_clear_object(&parser);
    }

    if (send_response)
    {
        g_socket_close(worker, NULL);
        g_clear_object(&worker);
    }
    g_free(buffer);
    g_free(params);

    return NULL;
}
