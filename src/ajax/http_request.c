#include <http_parser.h>
#include "http_request.h"

enum
{
    PROP_0,
    PROP_URL,
    PROP_PATH,
    PROP_QUERY_STRING,
    PROP_METHOD,
    PROP_HTTP_MAJOR,
    PROP_HTTP_MINOR,
    PROP_STATUS_CODE,
    PROP_KEEP_ALIVE,
    PROP_BODY,
    N_PROPERTIES
};

typedef struct _IpcamHttpRequestPrivate
{
    gchar *url;
    gchar *path;
    gchar *query_string;
    enum http_method method;
    guint http_major;
    guint http_minor;
    gchar *status_code;
    gboolean keep_alive;
    gchar *body;
    GHashTable *header_hash;
    gchar *header_field;
} IpcamHttpRequestPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamHttpRequest, ipcam_http_request, G_TYPE_OBJECT)

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_http_request_finalize(GObject *object)
{
    IpcamHttpRequest *self = IPCAM_HTTP_REQUEST(object);
    IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(self);
    g_free(priv->url);
    g_free(priv->path);
    g_free(priv->query_string);
    g_free(priv->status_code);
    g_free(priv->body);
    g_free(priv->header_field);
    g_hash_table_destroy(priv->header_hash);
    G_OBJECT_CLASS(ipcam_http_request_parent_class)->finalize(object);
}
static void destroy_notify(gpointer data)
{
    g_free(data);
}
static void ipcam_http_request_init(IpcamHttpRequest *self)
{
	IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(self);
    priv->header_hash = g_hash_table_new_full(g_str_hash, g_str_equal, destroy_notify, destroy_notify);
    priv->header_field = NULL;
}
static void ipcam_http_request_get_property(GObject    *object,
                                            guint       property_id,
                                            GValue     *value,
                                            GParamSpec *pspec)
{
    IpcamHttpRequest *self = IPCAM_HTTP_REQUEST(object);
    IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(self);
    switch(property_id)
    {
    case PROP_URL:
        {
            g_value_set_string(value, priv->url);
        }
        break;
    case PROP_PATH:
        {
            g_value_set_string(value, priv->path);
        }
        break;
    case PROP_QUERY_STRING:
        {
            g_value_set_string(value, priv->query_string);
        }
        break;
    case PROP_METHOD:
        {
            g_value_set_int(value, priv->method);
        }
        break;
    case PROP_HTTP_MAJOR:
        {
            g_value_set_int(value, priv->http_major);
        }
        break;
    case PROP_HTTP_MINOR:
        {
            g_value_set_int(value, priv->http_minor);
        }
        break;
    case PROP_STATUS_CODE:
        {
            g_value_set_string(value, priv->status_code);
        }
        break;
    case PROP_KEEP_ALIVE:
        {
            g_value_set_boolean(value, priv->keep_alive);
        }
        break;
    case PROP_BODY:
        {
            g_value_set_string(value, priv->body);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_http_request_set_property(GObject      *object,
                                            guint         property_id,
                                            const GValue *value,
                                            GParamSpec   *pspec)
{
    IpcamHttpRequest *self = IPCAM_HTTP_REQUEST(object);
    IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(self);
    switch(property_id)
    {
    case PROP_URL:
        {
            g_free(priv->url);
            priv->url = g_value_dup_string(value);
        }
        break;
    case PROP_PATH:
        {
            g_free(priv->path);
            priv->path = g_value_dup_string(value);
        }
        break;
    case PROP_QUERY_STRING:
        {
            g_free(priv->query_string);
            priv->query_string = g_value_dup_string(value);
        }
        break;
    case PROP_METHOD:
        {
            priv->method = g_value_get_int(value);
        }
        break;
    case PROP_HTTP_MAJOR:
        {
            priv->http_major = g_value_get_int(value);
        }
        break;
    case PROP_HTTP_MINOR:
        {
            priv->http_minor = g_value_get_int(value);
        }
        break;
    case PROP_STATUS_CODE:
        {
            g_free(priv->status_code);
            priv->status_code = g_value_dup_string(value);
        }
        break;
    case PROP_KEEP_ALIVE:
        {
            priv->keep_alive = g_value_get_boolean(value);
        }
        break;
    case PROP_BODY:
        {
            g_free(priv->body);
            priv->body = g_value_dup_string(value);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_http_request_class_init(IpcamHttpRequestClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = &ipcam_http_request_finalize;
    object_class->get_property = &ipcam_http_request_get_property;
    object_class->set_property = &ipcam_http_request_set_property;

    obj_properties[PROP_URL] =
        g_param_spec_string("url",
                            "url",
                            "url",
                            NULL, // default value
                            G_PARAM_READWRITE);
    obj_properties[PROP_PATH] =
        g_param_spec_string("path",
                            "path",
                            "path",
                            NULL, // default value
                            G_PARAM_READWRITE);
    obj_properties[PROP_QUERY_STRING] =
        g_param_spec_string("query-string",
                            "query string",
                            "query string",
                            NULL, // default value
                            G_PARAM_READWRITE);
    obj_properties[PROP_METHOD] =
        g_param_spec_int("method",
                         "method",
                         "method",
                         HTTP_DELETE,
                         HTTP_PURGE,
                         HTTP_GET, // default value
                         G_PARAM_READWRITE);
    obj_properties[PROP_HTTP_MAJOR] =
        g_param_spec_int("http-major",
                         "http major",
                         "http major",
                         1,
                         10,
                         1, // default value
                         G_PARAM_READWRITE);
    obj_properties[PROP_HTTP_MINOR] =
        g_param_spec_int("http-minor",
                         "http-minor",
                         "http minor",
                         0,
                         10,
                         1,// default value
                         G_PARAM_READWRITE);
    obj_properties[PROP_STATUS_CODE] =
        g_param_spec_string("status-code",
                            "status code",
                            "status code",
                            NULL, // default value
                            G_PARAM_READWRITE);
    obj_properties[PROP_KEEP_ALIVE] =
        g_param_spec_boolean("keep-alive",
                             "keep alive",
                             "keep alive",
                             FALSE, // default value
                             G_PARAM_READWRITE);
    obj_properties[PROP_BODY] =
        g_param_spec_string("body",
                            "body",
                            "body",
                            NULL, // default value
                            G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
void ipcam_http_request_add_header_field(IpcamHttpRequest *http_request, gchar *field)
{
    g_return_if_fail(IPCAM_IS_HTTP_REQUEST(http_request));
    IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(http_request);
    g_free(priv->header_field);
    priv->header_field = g_strdup(field);
}
void ipcam_http_request_add_header_value(IpcamHttpRequest *http_request, gchar *value)
{
    g_return_if_fail(IPCAM_IS_HTTP_REQUEST(http_request));
    IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(http_request);
    g_return_if_fail(priv->header_field);
    g_hash_table_insert(priv->header_hash, g_strdup(priv->header_field), g_strdup(value));
}
gchar *ipcam_http_request_get_header_value(IpcamHttpRequest *http_request, gchar *field)
{
    g_return_val_if_fail(IPCAM_IS_HTTP_REQUEST(http_request), NULL);
    IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(http_request);
    return g_strdup(g_hash_table_lookup(priv->header_hash, field));
}
gboolean ipcam_http_request_is_get(IpcamHttpRequest *http_request)
{
    g_return_val_if_fail(IPCAM_IS_HTTP_REQUEST(http_request), FALSE);
    IpcamHttpRequestPrivate *priv = ipcam_http_request_get_instance_private(http_request);
    return priv->method == HTTP_GET;
}
