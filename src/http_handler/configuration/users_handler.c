#include "http_users_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "http_query_string_parser.h"
#include "iconfig.h"
#include "../msg-handler/ipcam-users-handler.h"

G_DEFINE_TYPE(IpcamHttpUsersHandler, ipcam_http_users_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static gchar* do_get_action(IpcamIConfig *iconfig, GList *item_list)
{
    JsonBuilder *builder = NULL;
    JsonNode *req_node = NULL, *res_node = NULL;
    GList *item;
    JsonGenerator *generator;

    generator = json_generator_new();

    if (item_list)
    {
        builder = json_builder_new();
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
    }

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_USERS_MSG_HANDLER,
                                                      "app", iconfig, NULL);

    ipcam_message_handler_do_get(msg_handler, "get_users", req_node, &res_node);

    json_generator_set_root(generator, res_node);
    json_generator_set_pretty(generator, TRUE);

    gchar *result = json_generator_to_data(generator, NULL);;

    json_node_free(req_node);
    json_node_free(res_node);
    g_object_unref(msg_handler);
    if (builder)
    {
        g_object_unref(G_OBJECT(builder));
    }
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(get_users, HTTP_GET, "/api/1.0/users.json", http_request, http_response)
{
    IpcamIConfig *iconfig;
    IpcamHttpQueryStringParser *parser = NULL;
    gchar *query_string = NULL;
    GList *item_list = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;
    g_object_get(get_users, "app", &iconfig, NULL);
    g_object_get(http_request, "query-string", &query_string, NULL);
    if (query_string) 
    {
        parser = g_object_new(IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, NULL);
        query_hash = ipcam_http_query_string_parser_get(parser, query_string);
        if (query_hash)
        {
            item_list = g_hash_table_lookup(query_hash, "items[]");
        }
    }
    gchar *result = do_get_action(iconfig, item_list);
    g_object_set(http_response, "body", result, NULL);
    g_free(result);
    g_free(query_string);
    if (parser)
    {
        g_clear_object(&parser);
    }

    g_object_set(http_response,
                 "status", 200,
                 NULL);
    success = TRUE;
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

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_USERS_MSG_HANDLER,
                                                    "app", iconfig, NULL);

    ipcam_message_handler_do_put(msg_handler, "set_users", request, &response);

    json_generator_set_root(generator, response);
    json_generator_set_pretty(generator, TRUE);

    result = json_generator_to_data(generator, NULL);;

    json_node_free(response);
    g_object_unref(msg_handler);
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(put_users, HTTP_PUT, "/api/1.0/users.json", http_request, http_response)
{
    gchar *body = NULL;
    IpcamIConfig *iconfig;
    gboolean success = FALSE;
    g_print("fadsfasdf\n");
    
    g_object_get(put_users, "app", &iconfig, NULL);
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

static gchar* do_post_action(IpcamIConfig *iconfig, JsonNode *request)
{
    JsonNode *response;
    JsonGenerator *generator;
    gchar *result = NULL;

    generator = json_generator_new();

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_USERS_MSG_HANDLER,
                                                      "app", iconfig, NULL);

    ipcam_message_handler_do_post(msg_handler, "add_users", request, &response);

    json_generator_set_root(generator, response);
    json_generator_set_pretty(generator, TRUE);

    result = json_generator_to_data(generator, NULL);;

    json_node_free(response);
    g_object_unref(msg_handler);
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(post_users, HTTP_POST, "/api/1.0/users.json", http_request, http_response)
{
    gchar *body = NULL;
    IpcamIConfig *iconfig;
    gboolean success = FALSE;

    g_object_get(post_users, "app", &iconfig, NULL);
    g_object_get(http_request, "body", &body, NULL);
    if (body)
    {
        JsonParser *parser = json_parser_new();
        JsonNode *root_node;
        if (json_parser_load_from_data(parser, body, -1, NULL))
        {
            root_node = json_parser_get_root(parser);

            gchar *result = do_post_action(iconfig, root_node);
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

static gchar* do_delete_action(IpcamIConfig *iconfig, JsonNode *request)
{
    JsonNode *response;
    JsonGenerator *generator;
    gchar *result = NULL;

    generator = json_generator_new();

    IpcamMessageHandler *msg_handler = g_object_new(IPCAM_TYPE_USERS_MSG_HANDLER,
                                                      "app", iconfig, NULL);

    ipcam_message_handler_do_delete(msg_handler, "del_users", request, &response);

    json_generator_set_root(generator, response);
    json_generator_set_pretty(generator, TRUE);

    result = json_generator_to_data(generator, NULL);;

    json_node_free(response);
    g_object_unref(msg_handler);
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(delete_users, HTTP_DELETE, "/api/1.0/users.json", http_request, http_response)
{
    gchar *body = NULL;
    IpcamIConfig *iconfig;
    gboolean success = FALSE;

    g_object_get(delete_users, "app", &iconfig, NULL);
    g_object_get(http_request, "body", &body, NULL);
    if (body)
    {
        JsonParser *parser = json_parser_new();
        JsonNode *root_node;
        if (json_parser_load_from_data(parser, body, -1, NULL))
        {
            root_node = json_parser_get_root(parser);

            gchar *result = do_delete_action(iconfig, root_node);
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

static void ipcam_http_users_handler_init(IpcamHttpUsersHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_users);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), put_users);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), post_users);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), delete_users);
}

static void ipcam_http_users_handler_class_init(IpcamHttpUsersHandlerClass *klass)
{
}
