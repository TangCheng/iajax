#include "login_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"

G_DEFINE_TYPE(IpcamHttpLoginHandler, ipcam_http_login_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

#if 0
static gchar *do_get_action(IpcamIAjax *iajax, GList *user_list, GSocket *socket)
{
    JsonBuilder *builder;
    JsonNode *res_node = NULL;
    GList *user = g_list_first(user_list);
    const gchar *username = user->data;
    gchar *uuid;
    JsonGenerator *generator;
    
    builder = json_builder_new();
    generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "uuid");
    json_builder_begin_object(builder);
    if (username)
    {
        if (ipcam_iajax_apply_uuid(iajax, username, &uuid))
        {
            json_builder_set_member_name(builder, "username");
            json_builder_add_string_value(builder, username);
            json_builder_set_member_name(builder, "uuid");
            json_builder_add_string_value(builder, uuid);
            g_free(uuid);
        }
    }
    json_builder_end_object(builder);
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

START_HANDLER(get_login, HTTP_GET, "/api/1.0/login.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    IpcamHttpQueryStringParser *parser;
    gchar *query_string = NULL;
    GList *user_list = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;
    
    g_object_get(get_login, "app", &iajax, NULL);
    g_object_get(http_request, "query-string", &query_string, NULL);
    if (query_string) 
    {
        parser = g_object_new(IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, NULL);
        query_hash = ipcam_http_query_string_parser_get(parser, query_string);
        if (query_hash)
        {
            user_list = g_hash_table_lookup(query_hash, "user");
            if (user_list)
            {
                gchar *result = do_get_action(iajax, user_list, socket);
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
#endif

START_HANDLER(post_login, HTTP_POST, "/api/1.0/login.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    gchar *body = NULL;
    gchar *token = NULL;
    gchar *role = NULL;
    gchar *username = NULL;
    gchar *password = NULL;
    gboolean logon = FALSE;
    
    g_object_get(post_login, "app", &iajax, NULL);
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
            if (json_object_has_member(object, "token"))
            {
                token = g_strdup(json_object_get_string_member(object, "token"));
                logon = ipcam_iajax_check_login(iajax, token, &role);
            }
            else if (json_object_has_member(object, "username") &&
                     json_object_has_member(object, "password"))
            {
                username = json_object_get_string_member(object, "username");
                password = json_object_get_string_member(object, "password");
                logon = ipcam_iajax_login(iajax, username, password, &token, &role);
            }
        }
        g_object_unref(parser);
        g_free(body);
    }
    if (logon)
    {
        JsonBuilder *builder = json_builder_new();
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "success");
        json_builder_add_boolean_value(builder, TRUE);
        json_builder_set_member_name(builder, "token");
        json_builder_add_string_value(builder, token);
        json_builder_set_member_name(builder, "role");
        json_builder_add_string_value(builder, role);
        json_builder_end_object(builder);
        g_free(token);
        g_free(role);

        JsonGenerator *generator = json_generator_new();
        JsonNode *res_node = json_builder_get_root(builder);

        json_generator_set_root(generator, res_node);
        json_generator_set_pretty(generator, TRUE);

        gchar *result = json_generator_to_data(generator, NULL);
        g_object_set(http_response, "status", 200, "body", result, NULL);
        g_free(result);
                
        g_object_unref(G_OBJECT(generator));
        g_object_unref(G_OBJECT(builder));
    }
    else
    {
        g_object_set(http_response, "status", 200, NULL);
        ipcam_http_response_success(http_response, logon);
    }
    
    ret = TRUE;
}
END_HANDLER

static void ipcam_http_login_handler_init(IpcamHttpLoginHandler *self)
{
    //ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_login);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), post_login);
}

static void ipcam_http_login_handler_class_init(IpcamHttpLoginHandlerClass *klass)
{
}
