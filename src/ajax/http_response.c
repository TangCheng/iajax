#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <json-glib/json-glib.h>
#include "http_response.h"
#include "ctemplate.h"

enum
{
    PROP_0,
    PROP_HTTP_MAJOR,
    PROP_HTTP_MINOR,
    PROP_STATUS,
    PROP_BODY,
    N_PROPERTIES
};

typedef struct _IpcamHttpResponsePrivate
{
    guint http_major;
    guint http_minor;
    guint status;
    gchar *body;
} IpcamHttpResponsePrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamHttpResponse, ipcam_http_response, G_TYPE_OBJECT)

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_http_response_finalize(GObject *object)
{
    IpcamHttpResponsePrivate *priv = ipcam_http_response_get_instance_private(IPCAM_HTTP_RESPONSE(object));
    g_free(priv->body);
    G_OBJECT_CLASS(ipcam_http_response_parent_class)->finalize(object);
}
static void ipcam_http_response_init(IpcamHttpResponse *self)
{
}
static void ipcam_http_response_get_property(GObject    *object,
                                             guint       property_id,
                                             GValue     *value,
                                             GParamSpec *pspec)
{
    IpcamHttpResponse *self = IPCAM_HTTP_RESPONSE(object);
    IpcamHttpResponsePrivate *priv = ipcam_http_response_get_instance_private(self);
    switch(property_id)
    {
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
    case PROP_STATUS:
        {
            g_value_set_int(value, priv->status);
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
static void ipcam_http_response_set_property(GObject      *object,
                                             guint         property_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
{
    IpcamHttpResponse *self = IPCAM_HTTP_RESPONSE(object);
    IpcamHttpResponsePrivate *priv = ipcam_http_response_get_instance_private(self);
    switch(property_id)
    {
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
    case PROP_STATUS:
        {
            priv->status = g_value_get_int(value);
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
static void ipcam_http_response_class_init(IpcamHttpResponseClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = &ipcam_http_response_finalize;
    object_class->get_property = &ipcam_http_response_get_property;
    object_class->set_property = &ipcam_http_response_set_property;

    obj_properties[PROP_HTTP_MAJOR] =
        g_param_spec_int("http-major",
                         "http major version",
                         "http major version",
                         1,
                         10,
                         1,
                         G_PARAM_READWRITE);
    obj_properties[PROP_HTTP_MINOR] =
        g_param_spec_int("http-minor",
                         "http minor version",
                         "http minor version",
                         0,
                         10,
                         1,
                         G_PARAM_READWRITE);
    obj_properties[PROP_STATUS] =
        g_param_spec_int("status",
                         "http status code",
                         "http status code",
                         0,
                         65535,
                         200,
                         G_PARAM_READWRITE);
    obj_properties[PROP_BODY] =
        g_param_spec_string("body",
                            "html body",
                            "html body",
                            NULL,
                            G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
static TMPL_varlist *ipcam_http_response_get_varlist(IpcamHttpResponse *http_response)
{
    IpcamHttpResponsePrivate *priv = ipcam_http_response_get_instance_private(http_response);
    time_t now;
    TMPL_varlist *varlist = NULL;
    gchar str_value[32];

    snprintf(str_value, 16, "%u", priv->http_major);
    varlist = TMPL_add_var(varlist, "major", str_value, NULL);
    snprintf(str_value, 16, "%u", priv->http_minor);
    varlist = TMPL_add_var(varlist, "minor", str_value, NULL);
    snprintf(str_value, 16, "%u", priv->status);
    varlist = TMPL_add_var(varlist, "code", str_value, NULL);
    switch (priv->status)
    {
    case 200:
        varlist = TMPL_add_var(varlist, "status", "OK", NULL);
        break;
    case 400:
        varlist = TMPL_add_var(varlist, "status", "Bad Request", NULL);
        break;
    case 401:
        varlist = TMPL_add_var(varlist, "status", "Unauthorized", NULL);
        break;
    case 403:
        varlist = TMPL_add_var(varlist, "status", "Forbidden", NULL);
        break;
    case 404:
        varlist = TMPL_add_var(varlist, "status", "Not Found", NULL);
        break;
    case 408:
        varlist = TMPL_add_var(varlist, "status", "Request Timeout", NULL);
        break;
    case 500:
        varlist = TMPL_add_var(varlist, "status", "Internal Server Error", NULL);
        break;
    default:
        varlist = TMPL_add_var(varlist, "satus", "Unkonwn Status", NULL);
        break;
    }
    time(&now);
    strftime(str_value, 32, "%a, %d %b %Y %T %Z", gmtime(&now));
    varlist = TMPL_add_var(varlist, "datetime", str_value, NULL);
    if (priv->body)
    {
        snprintf(str_value, 16, "%u", (guint)strlen(priv->body));
        varlist = TMPL_add_var(varlist, "length", str_value, NULL);
        varlist = TMPL_add_var(varlist, "body", priv->body, NULL);
    }
    else
    {
        snprintf(str_value, 16, "%u", 0);
        varlist = TMPL_add_var(varlist, "length", str_value, NULL);
    }
    return varlist;
}
void ipcam_http_response_success(IpcamHttpResponse *http_response, gboolean success)
{
    g_return_if_fail(IPCAM_IS_HTTP_RESPONSE(http_response));
    JsonBuilder *builder = json_builder_new();
    JsonGenerator *generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "success");
    json_builder_add_boolean_value(builder, success);
    json_builder_end_object(builder);

    JsonNode *root = json_builder_get_root(builder);
    json_generator_set_root(generator, root);
    json_generator_set_pretty(generator, FALSE);
    
    const gchar *string = json_generator_to_data(generator, NULL);
    json_node_free(root);
    g_object_unref(generator);
    g_object_unref(builder);

    g_object_set(http_response, "status", 200, "body", string, NULL);
    g_free((gpointer)string);
}

void ipcam_http_response_write_string(IpcamHttpResponse *http_response, GSocket *socket)
{
    g_return_if_fail(IPCAM_IS_HTTP_RESPONSE(http_response));
    struct stat statbuf;
    FILE *out = fdopen(g_socket_get_fd(socket), "a");
    TMPL_varlist *varlist = ipcam_http_response_get_varlist(http_response);

    int fd_template = open("config/ajax.tmpl", O_RDONLY);
    if (fd_template < 0)
    {
        TMPL_write("config/ajax.tmpl", NULL, NULL, varlist, out, stderr);
        fflush(out);
        fclose(out);
        return;
    }
    else
    {
        fstat(fd_template, &statbuf);
        gchar *template = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd_template, 0);
        if (template != MAP_FAILED)
        {
            TMPL_write(NULL, template, NULL, varlist, out, stderr);
            munmap(template, statbuf.st_size);
        }
        else
        {
            TMPL_write("config/ajax.tmpl", NULL, NULL, varlist, out, stderr);
        }
        close(fd_template);
        fflush(out);
        fclose(out);
    }
    TMPL_free_varlist(varlist);
}
