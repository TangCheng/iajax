#define _GNU_SOURCE
#include <stdio.h>
#include "network_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"
#include "common.h"

G_DEFINE_TYPE(IpcamHttpNetworkHandler, ipcam_http_network_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

static JsonNode *query_network_address(IpcamIAjax *iajax)
{
	const gchar *token = ipcam_base_app_get_config(IPCAM_BASE_APP(iajax), "token");
	IpcamRequestMessage *req_msg;
	IpcamMessage *resp_msg;
    JsonBuilder *builder;
	JsonNode *addr_node = NULL;
	gboolean ret;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "address");
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_network",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(iajax), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, NULL, 5);
	ret = ipcam_base_app_wait_response(IPCAM_BASE_APP(iajax),
	                                   ipcam_request_message_get_id(req_msg),
	                                   5000, &resp_msg);
	if (ret)
	{
		JsonNode *resp_body;
		JsonObject *items_obj;

		g_object_get(G_OBJECT(resp_msg), "body", &resp_body, NULL);

		items_obj = json_object_get_object_member(json_node_get_object(resp_body), "items");
		addr_node = json_node_copy(json_object_get_member(items_obj, "address"));

		g_object_unref(resp_msg);
	}

	g_object_unref(req_msg);
	g_object_unref(builder);

	return addr_node;
}

static void do_get_action_address(IpcamIAjax *iajax, JsonBuilder *builder)
{
	JsonNode *addr_node = query_network_address(iajax);
	if (addr_node) {
		json_builder_set_member_name(builder, "address");
		json_builder_add_value(builder, addr_node);
	}
}

static void do_get_action_pppoe(IpcamIAjax *iajax, JsonBuilder *builder)
{
    GVariant *value = NULL;
    gchar *key = NULL;
    const gchar *pppoe_member[] = 
    {
        "username", "password"
    };
    gint i = 0;
    json_builder_set_member_name(builder, "pppoe");
    json_builder_begin_object(builder);
    for (i = 0; i < ARRAY_SIZE(pppoe_member); i++)
    {
        asprintf(&key, "network:pppoe:%s", pppoe_member[i]);
        value = ipcam_iajax_get_configuration(iajax, key);
        if (value)
        {
            add_value(builder, pppoe_member[i], value);
            g_variant_unref(value);
        }
        g_free(key);
    }
    json_builder_end_object(builder);
}

static void do_get_action_port(IpcamIAjax *iajax, JsonBuilder *builder)
{
    GVariant *value = NULL;
    gchar *key = NULL;
    const gchar *port_member[] = 
    {
        "http", "ftp", "rtsp"
    };
    gint i = 0;
    json_builder_set_member_name(builder, "port");
    json_builder_begin_object(builder);
    for (i = 0; i < ARRAY_SIZE(port_member); i++)
    {
        asprintf(&key, "network:port:%s", port_member[i]);
        value = ipcam_iajax_get_configuration(iajax, key);
        if (value)
        {
            add_value(builder, port_member[i], value);
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
        if (g_str_equal(name, "method"))
        {
            GVariant *value = NULL;
            value = ipcam_iajax_get_configuration(iajax, "network:method");
            if (value)
            {
                add_value(builder, name, value);
                g_variant_unref(value);
            }
        }
        else if (g_str_equal(name, "address"))
        {
            do_get_action_address(iajax, builder);
        }
        else if (g_str_equal(name, "pppoe"))
        {
            do_get_action_pppoe(iajax, builder);
        }
        else if (g_str_equal(name, "port"))
        {
            do_get_action_port(iajax, builder);
        }
        else
        {
            g_warn_if_reached();
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

START_HANDLER(get_network, HTTP_GET, "/api/1.0/network.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    IpcamHttpQueryStringParser *parser;
    gchar *query_string = NULL;
    GList *item_list = NULL;
    GHashTable *query_hash = NULL;
    gboolean success = FALSE;
    
    g_object_get(get_network, "app", &iajax, NULL);
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

START_HANDLER(put_network, HTTP_PUT, "/api/1.0/network.json", http_request, http_response, socket)
{
    ipcam_http_request_handler_do_put_action(put_network,
                                             http_request,
                                             http_response,
                                             socket,
                                             "set_network");
    
    ret = TRUE;
}
END_HANDLER

static void ipcam_http_network_handler_init(IpcamHttpNetworkHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_network);
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), put_network);
}

static void ipcam_http_network_handler_class_init(IpcamHttpNetworkHandlerClass *klass)
{
}
