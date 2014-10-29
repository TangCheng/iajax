#define _GNU_SOURCE
#include <string.h>
#include <messages.h>
#include "request_handler.h"
#include "iajax.h"

enum
{
    PROP_0,
    PROP_APP,
    N_PROPERTIES
};

typedef struct _IpcamHttpRequestHandlerPrivate
{
    IpcamIAjax *iajax;
    GList *handler_list;
} IpcamHttpRequestHandlerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamHttpRequestHandler, ipcam_http_request_handler, G_TYPE_OBJECT)

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_http_request_handler_finalize(GObject *object)
{
    IpcamHttpRequestHandlerPrivate *priv =
        ipcam_http_request_handler_get_instance_private(IPCAM_HTTP_REQUEST_HANDLER(object));
    g_list_free(priv->handler_list);
    G_OBJECT_CLASS(ipcam_http_request_handler_parent_class)->finalize(object);
}
static void ipcam_http_request_handler_init(IpcamHttpRequestHandler *self)
{
}
static void ipcam_http_request_handler_get_property(GObject    *object,
                                                    guint       property_id,
                                                    GValue     *value,
                                                    GParamSpec *pspec)
{
    IpcamHttpRequestHandler *self = IPCAM_HTTP_REQUEST_HANDLER(object);
    IpcamHttpRequestHandlerPrivate *priv = ipcam_http_request_handler_get_instance_private(self);
    switch(property_id)
    {
    case PROP_APP:
        {
            g_value_set_object(value, priv->iajax);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_http_request_handler_set_property(GObject      *object,
                                                    guint         property_id,
                                                    const GValue *value,
                                                    GParamSpec   *pspec)
{
    IpcamHttpRequestHandler *self = IPCAM_HTTP_REQUEST_HANDLER(object);
    IpcamHttpRequestHandlerPrivate *priv = ipcam_http_request_handler_get_instance_private(self);
    switch(property_id)
    {
    case PROP_APP:
        {
            priv->iajax = g_value_get_object(value);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_http_request_handler_class_init(IpcamHttpRequestHandlerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = &ipcam_http_request_handler_finalize;
    object_class->get_property = &ipcam_http_request_handler_get_property;
    object_class->set_property = &ipcam_http_request_handler_set_property;

    obj_properties[PROP_APP] =
        g_param_spec_object("app",
                            "application",
                            "application.",
                            IPCAM_IAJAX_TYPE,
                            G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
gboolean ipcam_http_request_handler_dispatch(IpcamHttpRequestHandler *http_request_handler,
                                             IpcamHttpRequest *http_request,
                                             IpcamHttpResponse *http_response,
                                             GSocket *socket)
{
    g_return_val_if_fail(IPCAM_IS_HTTP_REQUEST_HANDLER(http_request_handler), FALSE);
    g_return_val_if_fail(IPCAM_IS_HTTP_REQUEST(http_request), FALSE);
    g_return_val_if_fail(IPCAM_IS_HTTP_RESPONSE(http_response), FALSE);

    gboolean ret = FALSE;
    gchar *path = NULL;
    g_object_get(http_request, "path", &path, NULL);
    if (path == NULL) {
        return ret;
    }
    enum http_method method;
    g_object_get(http_request, "method", &method, NULL);
    
    IpcamHttpRequestHandlerPrivate *priv =
        ipcam_http_request_handler_get_instance_private(http_request_handler);
    GList *item = g_list_first(priv->handler_list);
    for (; item != NULL; item = g_list_next(item)) {
        handler_data *handler = item->data;
        if (handler->method == method) {
            gchar *m = strcasestr(path, handler->path);
            if (m) {
                ret = handler->func(http_request_handler, http_request, http_response, socket);
                break;
            }
        }
    }
    g_free(path);
    return ret;
}
void ipcam_http_request_handler_register(IpcamHttpRequestHandler *http_request_handler,
                                         handler_data *handler)
{
    g_return_if_fail(IPCAM_IS_HTTP_REQUEST_HANDLER(http_request_handler));
    IpcamHttpRequestHandlerPrivate *priv =
        ipcam_http_request_handler_get_instance_private(http_request_handler);
    priv->handler_list = g_list_append(priv->handler_list, handler);
}

void ipcam_http_request_handler_do_put_action(IpcamHttpRequestHandler *http_request_handler,
                                              IpcamHttpRequest *request, IpcamHttpResponse *response,
                                              GSocket *socket, const gchar *action)
{
    g_return_if_fail(IPCAM_IS_HTTP_REQUEST_HANDLER(http_request_handler));
    gchar *body = NULL;
    IpcamHttpRequestHandlerPrivate *priv =
        ipcam_http_request_handler_get_instance_private(http_request_handler);
    g_object_get(request, "body", &body, NULL);
    if (body)
    {
        JsonParser *parser = json_parser_new();
        JsonNode *root_node;
        if (json_parser_load_from_data(parser, body, -1, NULL))
        {
            root_node = json_node_copy(json_parser_get_root(parser));
            IpcamRequestMessage *req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
                                                        "action", action,
                                                        "body", root_node,
                                                        NULL);
            ipcam_iajax_set_configuration(priv->iajax, req_msg, response, socket);
            g_object_unref(req_msg);
        }
        g_object_unref(parser);
        g_free(body);
    }
}
void add_value(JsonBuilder *builder, const gchar *name, GVariant *value)
{
    g_return_if_fail(value);
    
    if (g_variant_is_of_type(value, G_VARIANT_TYPE_UINT32))
    {    
        json_builder_set_member_name(builder, name);
        json_builder_add_int_value(builder, g_variant_get_uint32(value));
    }
    else if (g_variant_is_of_type(value, G_VARIANT_TYPE_INT64))
    {    
        json_builder_set_member_name(builder, name);
        json_builder_add_int_value(builder, g_variant_get_int64(value));
    }
    else if (g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN))
    {
        json_builder_set_member_name(builder, name);
        json_builder_add_boolean_value(builder, g_variant_get_boolean(value));
    }
    else if (g_variant_is_of_type(value, G_VARIANT_TYPE_STRING))
    {
        json_builder_set_member_name(builder, name);
        json_builder_add_string_value(builder, g_variant_get_string(value, NULL));
    }
    else
    {
        g_warn_if_reached();
    }
}
