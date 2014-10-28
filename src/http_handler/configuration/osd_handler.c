#include "http_osd_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "http_query_string_parser.h"
#include "iconfig.h"
#include "../msg-handler/ipcam-osd-handler.h"
#include "common.h"

G_DEFINE_TYPE(IpcamHttpOsdHandler, ipcam_http_osd_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static gchar* do_get_action(IpcamIConfig *iconfig, GHashTable *item_hash)
{
    JsonBuilder *builder;
    JsonNode *req_node, *res_node = NULL;
    GList *item_list, *item;
    JsonGenerator *generator;
    const gchar *kv[] = {"master", "slave"};

    builder = json_builder_new();
    generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_array (builder);

    gchar *key = g_malloc0(32);
    gchar *value = g_malloc0(32);
    gint i = 0;
    for (i = 0; i < ARRAY_SIZE(kv); i++)
    {
        g_snprintf(key, 32, "items[%s][]", kv[i]);
        item_list = g_hash_table_lookup(item_hash, key);
        if (item_list)
        {
            for (item = g_list_first(item_list); item; item = g_list_next(item))
            {
                g_snprintf(value, 32, "%s:%s", kv[i], (gchar *)item->data);
                json_builder_add_string_value(builder, value);
            }
        }
    }
    g_free(value);
    g_free(key);
    
    json_builder_end_array(builder);
    json_builder_end_object(builder);

    req_node = json_builder_get_root(builder);

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_OSD_MSG_HANDLER,
                                                      "app", iconfig, NULL);

    ipcam_message_handler_do_get(msg_handler, "get_osd", req_node, &res_node);

    json_generator_set_root(generator, res_node);
    json_generator_set_pretty(generator, TRUE);

    gchar *result = json_generator_to_data(generator, NULL);;

    json_node_free(req_node);
    json_node_free(res_node);
    g_object_unref(msg_handler);
    g_object_unref(G_OBJECT(builder));
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(get_osd, HTTP_GET, "/api/1.0/osd.json", http_request, http_response)
{
    IpcamIConfig *iconfig;
    IpcamHttpQueryStringParser *parser;
    gchar *query_string = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;
    
    g_object_get(get_osd, "app", &iconfig, NULL);
    g_object_get(http_request, "query-string", &query_string, NULL);
    if (query_string) 
    {
        parser = g_object_new(IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, NULL);
        query_hash = ipcam_http_query_string_parser_get(parser, query_string);
        if (query_hash)
        {
            gchar *result = do_get_action(iconfig, query_hash);
            g_object_set(http_response, "body", result, NULL);
            g_free(result);

            g_object_set(http_response,
                         "status", 200,
                         NULL);
            success = TRUE;
        }
        g_free(query_string);
        g_clear_object(&parser);
    }
    if (!success)
    {
        ipcam_http_response_success(http_response, success);
    }
    ret = TRUE;
}
END_HANDLER

static gchar* do_put_action(IpcamIConfig *iconfig, JsonNode *request)
{
    JsonNode *response;
    JsonGenerator *generator;
    gchar *result = NULL;

    generator = json_generator_new();

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_OSD_MSG_HANDLER,
                                                      "app", iconfig, NULL);

    ipcam_message_handler_do_put(msg_handler, "set_osd", request, &response);

    json_generator_set_root(generator, response);
    json_generator_set_pretty(generator, TRUE);

    result = json_generator_to_data(generator, NULL);;

    json_node_free(response);
    g_object_unref(msg_handler);
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(put_osd, HTTP_PUT, "/api/1.0/osd.json", http_request, http_response)
{
    gchar *body = NULL;
    IpcamIConfig *iconfig;
    gboolean success = FALSE;

    g_object_get(put_osd, "app", &iconfig, NULL);
    g_object_get(http_request, "body", &body, NULL);
    if (body)
    {
        JsonParser *parser = json_parser_new();
        JsonNode *root_node;
        if (json_parser_load_from_data(parser, body, -1, NULL))
        {
            root_node = json_parser_get_root(parser);

            gchar *result = do_put_action(iconfig, root_node);
            g_object_set(http_response, "body", result, NULL);
            g_free(result);

            g_object_set(http_response,
                         "status", 200,
                         NULL);
            success = TRUE;
        }
        g_object_unref(parser);
        g_free(body);
    }
    if (!success)
    {
        ipcam_http_response_success(http_response, success);
    }
    ret = TRUE;
}
END_HANDLER

static void ipcam_http_osd_handler_init(IpcamHttpOsdHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_osd);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), put_osd);
}

static void ipcam_http_osd_handler_class_init(IpcamHttpOsdHandlerClass *klass)
{
}
