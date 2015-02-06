#define _GNU_SOURCE
#include <stdio.h>
#include "privacy_block_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"
#include "common.h"

G_DEFINE_TYPE(IpcamHttpPrivacyBlockHandler, ipcam_http_privacy_block_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static void do_get_action_rect(IpcamIAjax *iajax, JsonBuilder *builder, const gchar *name)
{
    GVariant *value = NULL;
    gchar *key = NULL;
    gint i = 0;
    json_builder_set_member_name(builder, "rect");
    json_builder_begin_object(builder);

    for (i = 0; i < 4; i++)
    {        
        asprintf(&key, "privacy_block:%s:rect:%s", name, rect_info[i]);
        value = ipcam_iajax_get_configuration(iajax, key);
        if (value)
        {
            add_value(builder, rect_info[i], value);
            g_variant_unref(value);
        }
        g_free(key);
    }

    json_builder_end_object(builder);
}

static void do_get_action_color(IpcamIAjax *iajax, JsonBuilder *builder, const gchar *name)
{
    GVariant *value = NULL;
    gchar *key = NULL;
    gint i = 0;
    json_builder_set_member_name(builder, "color");
    json_builder_begin_object(builder);

    for (i = 0; i < ENUM_COLOR_MEMBER_LAST; i++)
    {        
        asprintf(&key, "privacy_block:%s:color:%s", name, color_member[i]);
        value = ipcam_iajax_get_configuration(iajax, key);
        if (value)
        {
            add_value(builder, color_member[i], value);
            g_variant_unref(value);
        }
        g_free(key);
    }

    json_builder_end_object(builder);
}

static gchar* do_get_action(IpcamIAjax *iajax, GList *item_list)
{
    JsonBuilder *builder;
    JsonNode *res_node = NULL;
    GList *item;
    JsonGenerator *generator;

    builder = json_builder_new();
    generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_object(builder);
    for (item = g_list_first(item_list); item; item = g_list_next(item))
    {
        const gchar *name = item->data;
        json_builder_set_member_name(builder, name);
        json_builder_begin_object(builder);

        GVariant *value = NULL;
        gchar *key = NULL;

        asprintf(&key, "privacy_block:%s:enable", name);
        value = ipcam_iajax_get_configuration(iajax, key);
        if (value)
        {
            add_value(builder, "enable", value);
            g_variant_unref(value);
        }
        g_free(key);
        do_get_action_rect(iajax, builder, name);
        do_get_action_color(iajax, builder, name);
        
        json_builder_end_object(builder);
    }
    json_builder_end_object(builder);
    json_builder_end_object(builder);

    res_node = json_builder_get_root(builder);
    json_generator_set_root(generator, res_node);
    json_generator_set_pretty(generator, TRUE);

    gchar *result = json_generator_to_data(generator, NULL);;

    json_node_free(res_node);
    g_object_unref(G_OBJECT(builder));
    g_object_unref(G_OBJECT(generator));

    return result;
}

START_HANDLER(get_privacy_block, HTTP_GET, "/api/1.0/privacy_block.json", http_request, http_response, socket)
{
    IpcamHttpQueryStringParser *parser;
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
            if (item_list)
            {
                IpcamIAjax *iajax;
                g_object_get(get_privacy_block, "app", &iajax, NULL);
                gchar *result = do_get_action(iajax, item_list);
                g_clear_object(&iajax);
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

START_HANDLER(put_privacy_block, HTTP_PUT, "/api/1.0/privacy_block.json", http_request, http_response, socket)
{
    ipcam_http_request_handler_do_put_action(put_privacy_block,
                                             http_request,
                                             http_response,
                                             socket,
                                             "set_privacy_block");
    
    ret = TRUE;
}
END_HANDLER

static void ipcam_http_privacy_block_handler_init(IpcamHttpPrivacyBlockHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_privacy_block);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), put_privacy_block);
}

static void ipcam_http_privacy_block_handler_class_init(IpcamHttpPrivacyBlockHandlerClass *klass)
{
}
