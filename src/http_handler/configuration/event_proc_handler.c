#include "http_event_proc_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "http_query_string_parser.h"
#include "iconfig.h"
#include "../msg-handler/ipcam-event-proc-handler.h"

G_DEFINE_TYPE(IpcamHttpEventProcHandler, ipcam_http_event_proc_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static gchar* do_get_action(IpcamIConfig *iconfig, GList *item_list)
{
    JsonBuilder *builder;
    JsonNode *req_node, *res_node = NULL;
    GList *item;
    JsonGenerator *generator;

    builder = json_builder_new();
    generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_array (builder);
    for (item = g_list_first(item_list); item; item = g_list_next(item))
    {
        json_builder_add_string_value(builder, item->data);
    }
    json_builder_end_array(builder);
    json_builder_end_object(builder);

    req_node = json_builder_get_root(builder);

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_EVENT_PROC_MSG_HANDLER,
                                                      "app", iconfig, NULL);

    ipcam_message_handler_do_get(msg_handler, "get_event_proc", req_node, &res_node);

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

START_HANDLER(get_event_proc, HTTP_GET, "/api/1.0/event_proc.json", http_request, http_response)
{
    IpcamIConfig *iconfig;
    IpcamHttpQueryStringParser *parser;
    gchar *query_string = NULL;
    GList *item_list = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;

    g_object_get(get_event_proc, "app", &iconfig, NULL);
    g_object_get(http_request, "query-string", &query_string, NULL);
    if (query_string) 
    {
        parser = g_object_new(IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, NULL);
        query_hash = ipcam_http_query_string_parser_get(parser, query_string);
        if (query_hash)
        {
            item_list = g_hash_table_lookup(query_hash, "items[]");
            if (item_list)
            {
                gchar *result = do_get_action(iconfig, item_list);
                g_object_set(http_response, "body", result, NULL);
                g_free(result);

                g_object_set(http_response,
                             "status", 200,
                             NULL);
                success = TRUE;
            }
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

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_EVENT_PROC_MSG_HANDLER,
                                                      "app", iconfig, NULL);

    ipcam_message_handler_do_put(msg_handler, "set_event_proc", request, &response);

    json_generator_set_root(generator, response);
    json_generator_set_pretty(generator, TRUE);

    result = json_generator_to_data(generator, NULL);;

    json_node_free(response);
    g_object_unref(msg_handler);
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(put_event_proc, HTTP_PUT, "/api/1.0/event_proc.json", http_request, http_response)
{
    gchar *body = NULL;
    IpcamIConfig *iconfig;
    gboolean success = FALSE;

    g_object_get(put_event_proc, "app", &iconfig, NULL);
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

static void ipcam_http_event_proc_handler_init(IpcamHttpEventProcHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_event_proc);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), put_event_proc);
}

static void ipcam_http_event_proc_handler_class_init(IpcamHttpEventProcHandlerClass *klass)
{
}
