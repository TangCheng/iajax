#include "system_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"

G_DEFINE_TYPE(IpcamHttpSystemHandler, ipcam_http_system_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

START_HANDLER(post_system, HTTP_POST, "/api/1.0/system.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    gchar *body = NULL;
    gchar *action = NULL;
    
    g_object_get(post_system, "app", &iajax, NULL);
    g_object_get(http_request, "body", &body, NULL);
    if (body)
    {
        JsonParser *parser = json_parser_new();
        JsonNode *root_node;
        JsonObject *object;
        if (json_parser_load_from_data(parser, body, -1, NULL))
        {
            root_node = json_parser_get_root(parser);
            object = json_node_get_object(root_node);
            if (json_object_has_member(object, "action"))
            {
                action = g_strdup(json_object_get_string_member(object, "action"));
                if (g_str_equal(action , "reboot"))
                {
                    sysutils_reboot();
                }
                else if (g_str_equal(action, "soft_reset"))
                {
                }
                else if (g_str_equal(action, "hard_reset"))
                {
                }
                g_object_set(http_response, "status", 200, NULL);
            }
        }
        g_object_unref(parser);
        g_free(body);
    }
    
    ret = TRUE;
}
END_HANDLER

static void ipcam_http_system_handler_init(IpcamHttpSystemHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), post_system);
}

static void ipcam_http_system_handler_class_init(IpcamHttpSystemHandlerClass *klass)
{
}
