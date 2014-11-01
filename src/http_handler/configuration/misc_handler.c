#define _GNU_SOURCE
#include <stdio.h>
#include "misc_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"

G_DEFINE_TYPE(IpcamHttpMiscHandler, ipcam_http_misc_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

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
        GVariant *value = NULL;
        gchar *key = NULL;
        if (g_str_equal(name, "main_stream_url"))
        {
            asprintf(&key, "misc:rtsp_auth", name);
            value = ipcam_iajax_get_configuration(iajax, key);
            if (value)
            {
                add_value(builder, "rtsp_auth", value);
                g_variant_unref(value);
            }
            g_free(key);
            
            asprintf(&key, "network:port:rtsp", name);
            value = ipcam_iajax_get_configuration(iajax, key);
            if (value)
            {
                add_value(builder, "port", value);
                g_variant_unref(value);
            }
            g_free(key);

            asprintf(&key, "video:main_profile:stream_path", name);
            value = ipcam_iajax_get_configuration(iajax, key);
            if (value)
            {
                add_value(builder, "stream_path", value);
                g_variant_unref(value);
            }
            g_free(key);
        }
        else if (g_str_equal(name, "sub_stream_url"))
        {
            asprintf(&key, "misc:rtsp_auth", name);
            value = ipcam_iajax_get_configuration(iajax, key);
            if (value)
            {
                add_value(builder, "rtsp_auth", value);
                g_variant_unref(value);
            }
            g_free(key);
            
            asprintf(&key, "network:port:rtsp", name);
            value = ipcam_iajax_get_configuration(iajax, key);
            if (value)
            {
                add_value(builder, "port", value);
                g_variant_unref(value);
            }
            g_free(key);

            asprintf(&key, "video:sub_profile:stream_path", name);
            value = ipcam_iajax_get_configuration(iajax, key);
            if (value)
            {
                add_value(builder, "stream_path", value);
                g_variant_unref(value);
            }
            g_free(key);
        }
        else
        { 
            asprintf(&key, "misc:%s", name);
            value = ipcam_iajax_get_configuration(iajax, key);
            if (value)
            {
                add_value(builder, name, value);
                g_variant_unref(value);
            }
            g_free(key);
        }
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

START_HANDLER(get_misc, HTTP_GET, "/api/1.0/misc.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    IpcamHttpQueryStringParser *parser;
    gchar *query_string = NULL;
    GList *item_list = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;

    g_object_get(get_misc, "app", &iajax, NULL);
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
                gchar *result = do_get_action(iajax, item_list);
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

START_HANDLER(put_misc, HTTP_PUT, "/api/1.0/misc.json", http_request, http_response, socket)
{
    ipcam_http_request_handler_do_put_action(put_misc,
                                             http_request,
                                             http_response,
                                             socket,
                                             "set_misc");
    
    ret = TRUE;
}
END_HANDLER

static void ipcam_http_misc_handler_init(IpcamHttpMiscHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_misc);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), put_misc);
}

static void ipcam_http_misc_handler_class_init(IpcamHttpMiscHandlerClass *klass)
{
}
