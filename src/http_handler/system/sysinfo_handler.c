#include "sysinfo_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"
#include "sysutils.h"

G_DEFINE_TYPE(IpcamHttpSysinfoHandler, ipcam_http_sysinfo_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

void get_cpu_info(JsonBuilder *builder)
{
    gchar *info = NULL;
    guint usage = 0;

    usage = sysutils_get_cpu_usage();
    
    if (sysutils_get_cpu_info(&info))
    {
        json_builder_set_member_name(builder, "cpu");
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "info");
        json_builder_add_string_value(builder, info);
        json_builder_set_member_name(builder, "usage");
        json_builder_add_int_value(builder, usage);
        json_builder_end_object(builder);
    }
    g_free(info);
}

void get_mem_info(JsonBuilder *builder)
{
    gchar *totalram = NULL;
    gchar *usedram = NULL;
    gchar *freeram = NULL;
    guint usage = 0;
    
    if (sysutils_get_memory_info(&totalram, &usedram, &freeram, &usage))
    {
        json_builder_set_member_name(builder, "memory");
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "total");
        json_builder_add_string_value(builder, totalram);
        json_builder_set_member_name(builder, "used");
        json_builder_add_string_value(builder, usedram);
        json_builder_set_member_name(builder, "free");
        json_builder_add_string_value(builder, freeram);
        json_builder_set_member_name(builder, "usage");
        json_builder_add_int_value(builder, usage);
        json_builder_end_object(builder);
        g_free(totalram);
        g_free(usedram);
        g_free(freeram);
    }
}

void get_eth_info(JsonBuilder *builder)
{
    gchar *band_width = NULL;
    gchar *tx = NULL;
    gchar *rx = NULL;
    guint tx_speed;
    guint rx_speed;
    
    if (sysutils_get_net_info(&band_width, &tx, &rx, &tx_speed, &rx_speed))
    {
        json_builder_set_member_name(builder, "net");
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "bandwidth");
        json_builder_add_string_value(builder, band_width);
        json_builder_set_member_name(builder, "tx");
        json_builder_add_string_value(builder, tx);
        json_builder_set_member_name(builder, "rx");
        json_builder_add_string_value(builder, rx);
        json_builder_set_member_name(builder, "tx_speed");
        json_builder_add_int_value(builder, tx_speed);
        json_builder_set_member_name(builder, "rx_speed");
        json_builder_add_int_value(builder, rx_speed);
        json_builder_end_object(builder);
        g_free(band_width);
        g_free(tx);
        g_free(rx);
    }
}

static gchar *do_get_action(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    JsonNode *res_node = NULL;
    JsonGenerator *generator;
    gint status = 0;
    
    builder = json_builder_new();
    generator = json_generator_new();

    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "sysinfo");
    json_builder_begin_object(builder);

    gchar *uptime = NULL;
    if (sysutils_get_uptime(&uptime))
    {
        json_builder_set_member_name(builder, "uptime");
        json_builder_add_string_value(builder, uptime);
        g_free(uptime);
    }

    get_cpu_info(builder);
    get_mem_info(builder);
    get_eth_info(builder);
    
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

START_HANDLER(get_sysinfo, HTTP_GET, "/api/1.0/sysinfo.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    IpcamHttpQueryStringParser *parser;
    gboolean success = FALSE;
    
    g_object_get(get_sysinfo, "app", &iajax, NULL);

    gchar *result = do_get_action(iajax);
    g_object_set(http_response, "body", result, NULL);
    g_free(result);

    g_object_set(http_response,
                 "status", 200,
                 NULL);

    ret = TRUE;
}
END_HANDLER

static void ipcam_http_sysinfo_handler_init(IpcamHttpSysinfoHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_sysinfo);
}

static void ipcam_http_sysinfo_handler_class_init(IpcamHttpSysinfoHandlerClass *klass)
{
}
