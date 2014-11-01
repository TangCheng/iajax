#include "upgrade_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"
#include "sysutils.h"

G_DEFINE_TYPE(IpcamHttpUpgradeHandler, ipcam_http_upgrade_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static gchar *do_get_action(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    JsonNode *res_node = NULL;
    JsonGenerator *generator;
    gint status = 0;
    
    builder = json_builder_new();
    generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "status");
    status = sysutils_get_upgrade_status();
    json_builder_add_int_value(builder, status);
    json_builder_end_object(builder);

    res_node = json_builder_get_root(builder);

    json_generator_set_root(generator, res_node);
    json_generator_set_pretty(generator, TRUE);

    gchar *result = json_generator_to_data(generator, NULL);

    json_node_free(res_node);
    g_object_unref(G_OBJECT(builder));
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(get_upgrade, HTTP_GET, "/api/1.0/upgrade.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    IpcamHttpQueryStringParser *parser;
    gboolean success = FALSE;
    
    g_object_get(get_upgrade, "app", &iajax, NULL);

    gchar *result = do_get_action(iajax);
    g_object_set(http_response, "body", result, NULL);
    g_free(result);

    g_object_set(http_response,
                 "status", 200,
                 NULL);

    ret = TRUE;
}
END_HANDLER

static void ipcam_http_upgrade_handler_init(IpcamHttpUpgradeHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_upgrade);
}

static void ipcam_http_upgrade_handler_class_init(IpcamHttpUpgradeHandlerClass *klass)
{
}
