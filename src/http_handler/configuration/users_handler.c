#define _GNU_SOURCE
#include <stdio.h>
#include "users_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"

G_DEFINE_TYPE(IpcamHttpUsersHandler, ipcam_http_users_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static gchar* do_get_action(IpcamIAjax *iajax, GList *item_list)
{
    JsonBuilder *builder = NULL;
    JsonNode *res_node = NULL;
    GList *item;
    JsonGenerator *generator;
    GList *user;
    GList *users = ipcam_iajax_get_users(iajax);

    users = g_list_sort(users, (GCompareFunc)g_strcmp0);
    builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_array(builder);
    for (user = g_list_first(users); user; user = g_list_next(user))
    {
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "username");
        json_builder_add_string_value(builder, user->data);
        if (item_list)
        {
            for (item = g_list_first(item_list); item; item = g_list_next(item))
            {
                if (g_str_equal(item->data, "password"))
                {
                    gchar *pwd = ipcam_iajax_get_user_pwd(iajax, user->data);
                    json_builder_set_member_name(builder, "password");
                    json_builder_add_string_value(builder, pwd);
                }
                else if (g_str_equal(item->data, "role"))
                {
                    gchar *role = ipcam_iajax_get_user_role(iajax, user->data);
                    json_builder_set_member_name(builder, "role");
                    json_builder_add_string_value(builder, role);
                    g_free(role);
                }
                else
                {
                    g_warn_if_reached();
                }
            }
        }
        json_builder_end_object(builder);
    }
    g_list_free_full(users, g_free);
    json_builder_end_array(builder);
    json_builder_end_object(builder);

    generator = json_generator_new();
    res_node = json_builder_get_root(builder);
    json_generator_set_root(generator, res_node);
    json_generator_set_pretty(generator, TRUE);

    gchar *result = json_generator_to_data(generator, NULL);;

    json_node_free(res_node);
    g_object_unref(G_OBJECT(builder));
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(get_users, HTTP_GET, "/api/1.0/users.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    IpcamHttpQueryStringParser *parser = NULL;
    gchar *query_string = NULL;
    GList *item_list = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;
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
    g_object_get(get_users, "app", &iajax, NULL);
    gchar *result = do_get_action(iajax, item_list);
    g_clear_object(&iajax);
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

START_HANDLER(put_users, HTTP_PUT, "/api/1.0/users.json", http_request, http_response, socket)
{
    ipcam_http_request_handler_do_put_action(put_users,
                                             http_request,
                                             http_response,
                                             socket,
                                             "set_users");
    
    ret = TRUE;
}
END_HANDLER

START_HANDLER(post_users, HTTP_POST, "/api/1.0/users.json", http_request, http_response, socket)
{
    ipcam_http_request_handler_do_put_action(post_users,
                                             http_request,
                                             http_response,
                                             socket,
                                             "add_users");
    
    ret = TRUE;
}
END_HANDLER

START_HANDLER(delete_users, HTTP_DELETE, "/api/1.0/users.json", http_request, http_response, socket)
{
    ipcam_http_request_handler_do_put_action(delete_users,
                                             http_request,
                                             http_response,
                                             socket,
                                             "del_users");
    
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
