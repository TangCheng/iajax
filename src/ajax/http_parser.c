#include <http_parser.h>
#include <assert.h>
#include <string.h>
#include "http_parser.h"

G_DEFINE_TYPE(IpcamHttpParser, ipcam_http_parser, G_TYPE_OBJECT)

static http_parser* ipcam_http_parser_parser_init(enum http_parser_type type);
static void ipcam_http_parser_parser_free(http_parser *parser);
static int ipcam_http_parser_request_url_cb (http_parser *p, const char *buf, size_t len);
static int ipcam_http_parser_header_field_cb (http_parser *p, const char *buf, size_t len);
static int ipcam_http_parser_header_value_cb (http_parser *p, const char *buf, size_t len);
static int ipcam_http_parser_body_cb (http_parser *p, const char *buf, size_t len);
static int ipcam_http_parser_message_begin_cb (http_parser *p);
static int ipcam_http_parser_headers_complete_cb (http_parser *p);
static int ipcam_http_parser_message_complete_cb (http_parser *p);
static int ipcam_http_parser_response_status_cb (http_parser *p, const char *buf, size_t len);

static http_parser_settings settings =
{
    .on_message_begin = ipcam_http_parser_message_begin_cb,
    .on_header_field = ipcam_http_parser_header_field_cb,
    .on_header_value = ipcam_http_parser_header_value_cb,
    .on_url = ipcam_http_parser_request_url_cb,
    .on_status = ipcam_http_parser_response_status_cb,
    .on_body = ipcam_http_parser_body_cb,
    .on_headers_complete = ipcam_http_parser_headers_complete_cb,
    .on_message_complete = ipcam_http_parser_message_complete_cb
};

static void ipcam_http_parser_init(IpcamHttpParser *self)
{
}
static void ipcam_http_parser_class_init(IpcamHttpParserClass *klass)
{
}
IpcamHttpRequest *ipcam_http_parser_get_request(IpcamHttpParser *self, gchar *http_buffer, gssize len)
{
    g_return_val_if_fail(IPCAM_IS_HTTP_PARSER(self), NULL);
    IpcamHttpRequest *request = g_object_new(IPCAM_HTTP_REQUEST_TYPE, NULL);
    http_parser *parser = ipcam_http_parser_parser_init(HTTP_REQUEST);
    parser->data = request;
    gssize nparsed = http_parser_execute(parser, &settings, http_buffer, len);
    nparsed = nparsed; /* clear compile warning. */
    ipcam_http_parser_parser_free(parser);
    return request;
}
static http_parser* ipcam_http_parser_parser_init(enum http_parser_type type)
{
    http_parser *parser = g_new(http_parser, 1);
    http_parser_init(parser, type);
    return parser;
}
static void ipcam_http_parser_parser_free(http_parser *parser)
{
    g_free(parser);
}
static int ipcam_http_parser_request_url_cb (http_parser *p, const char *buf, size_t len)
{
    IpcamHttpRequest *request = IPCAM_HTTP_REQUEST(p->data);
    g_object_set(request, "url", buf, NULL);
    struct http_parser_url u;

    if (!http_parser_parse_url(buf, len, 0, &u)) {
        if (u.field_set & (1 << UF_PATH))
        {
            gchar *path = g_new(gchar, u.field_data[UF_PATH].len + 1);
            strncpy(path, buf + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len);
            path[u.field_data[UF_PATH].len] = '\0';
            g_object_set(request, "path", path, NULL);
            g_free(path);
        }
        if (u.field_set & (1 << UF_QUERY))
        {
            gchar *query_string = g_new(gchar, u.field_data[UF_QUERY].len + 1);
            strncpy(query_string, buf + u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len);
            query_string[u.field_data[UF_QUERY].len] = '\0';
            g_object_set(request, "query-string", query_string, NULL);
            g_free(query_string);
        }
    }
    return 0;
}
static int ipcam_http_parser_header_field_cb (http_parser *p, const char *buf, size_t len)
{
    IpcamHttpRequest *request = IPCAM_HTTP_REQUEST(p->data);
    ipcam_http_request_add_header_field(request, (char *)buf);
    return 0;
}
static int ipcam_http_parser_header_value_cb (http_parser *p, const char *buf, size_t len)
{
    IpcamHttpRequest *request = IPCAM_HTTP_REQUEST(p->data);
    ipcam_http_request_add_header_value(request, (char *)buf);
    return 0;
}
static int ipcam_http_parser_body_cb (http_parser *p, const char *buf, size_t len)
{
    IpcamHttpRequest *request = IPCAM_HTTP_REQUEST(p->data);
    g_object_set(request, "body", buf, NULL);
    return 0;
}
static int ipcam_http_parser_message_begin_cb (http_parser *p)
{
    return 0;
}
static int ipcam_http_parser_headers_complete_cb (http_parser *p)
{
    IpcamHttpRequest *request = IPCAM_HTTP_REQUEST(p->data);
    g_object_set(request,
                 "http-major", p->http_major,
                 "http-minor", p->http_minor,
                 "method", p->method,
                 "keep-alive", http_should_keep_alive(p),
                 NULL);
    return 0;
}
static int ipcam_http_parser_message_complete_cb (http_parser *p)
{
    return 0;
}
static int ipcam_http_parser_response_status_cb (http_parser *p, const char *buf, size_t len)
{
    return 0;
}
