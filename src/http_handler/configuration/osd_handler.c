#define _GNU_SOURCE
#include <stdio.h>
#include "osd_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"
#include "common.h"

G_DEFINE_TYPE(IpcamHttpOsdHandler, ipcam_http_osd_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static void do_get_action_osd(IpcamIAjax *iajax, JsonBuilder *builder, const gchar *chn, const gchar *type)
{
    GVariant *value = NULL;
    gchar *key = NULL;
    const gchar *osd_member[] = 
    {
        "isshow", "size", "left", "top"
    };
    gint i = 0;
    json_builder_set_member_name(builder, type);
    json_builder_begin_object(builder);
    for (i = 0; i < ARRAY_SIZE(osd_member); i++)
    {
        asprintf(&key, "osd:%s:%s:%s", chn, type, osd_member[i]);
        value = ipcam_iajax_get_configuration(iajax, key);
        if (value)
        {
            add_value(builder, osd_member[i], value);
            g_variant_unref(value);
        }
        g_free(key);
    }
    json_builder_set_member_name(builder, "color");
    json_builder_begin_object(builder);
    for (i = 0; i < ENUM_COLOR_MEMBER_LAST; i++)
    {
        asprintf(&key, "osd:%s:%s:color:%s", chn, type, color_member[i]);
        value = ipcam_iajax_get_configuration(iajax, key);
        if (value)
        {
            add_value(builder, color_member[i], value);
            g_variant_unref(value);
        }
        g_free(key);
    }
    json_builder_end_object(builder);
    json_builder_end_object(builder);
}

static gchar* do_get_action(IpcamIAjax *iajax, GHashTable *item_hash)
{
    JsonBuilder *builder;
    JsonNode *res_node = NULL;
    GList *item_list, *item;
    JsonGenerator *generator;
    const gchar *kv[] = {"master", "slave"};

    builder = json_builder_new();
    generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_object(builder);

    gchar *key = g_malloc0(32);
    gint i = 0;
    for (i = 0; i < ARRAY_SIZE(kv); i++)
    {
        g_snprintf(key, 32, "items[%s][]", kv[i]);
        item_list = g_hash_table_lookup(item_hash, key);
        if (item_list)
        {
            json_builder_set_member_name(builder, kv[i]);
            json_builder_begin_object(builder);
            for (item = g_list_first(item_list); item; item = g_list_next(item))
            {
                do_get_action_osd(iajax, builder, kv[i], item->data);
            }
            json_builder_end_object(builder);
        }
    }
    g_free(key);
    
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

START_HANDLER(get_osd, HTTP_GET, "/api/1.0/osd.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    IpcamHttpQueryStringParser *parser;
    gchar *query_string = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;
    
    g_object_get(get_osd, "app", &iajax, NULL);
    g_object_get(http_request, "query-string", &query_string, NULL);
    if (query_string) 
    {
        parser = g_object_new(IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, NULL);
        query_hash = ipcam_http_query_string_parser_get(parser, query_string);
        if (query_hash)
        {
            gchar *result = do_get_action(iajax, query_hash);
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

START_HANDLER(put_osd, HTTP_PUT, "/api/1.0/osd.json", http_request, http_response, socket)
{
    ipcam_http_request_handler_do_put_action(put_osd,
                                             http_request,
                                             http_response,
                                             socket,
                                             "set_osd");
    
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
