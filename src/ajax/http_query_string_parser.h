#ifndef __HTTP_QUERY_STRING_PARSER_H__
#define __HTTP_QUERY_STRING_PARSER_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_HTTP_QUERY_STRING_PARSER_TYPE (ipcam_http_query_string_parser_get_type())
#define IPCAM_HTTP_QUERY_STRING_PARSER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, IpcamHttpQueryStringParser))
#define IPCAM_HTTP_QUERY_STRING_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, IpcamHttpQueryStringParserClass))
#define IPCAM_IS_HTTP_QUERY_STRING_PARSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_QUERY_STRING_PARSER_TYPE))
#define IPCAM_IS_HTTP_QUERY_STRING_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_QUERY_STRING_PARSER_TYPE))
#define IPCAM_HTTP_QUERY_STRING_PARSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_QUERY_STRING_PARSER_TYPE, IpcamHttpQueryStringParserClass))

typedef struct _IpcamHttpQueryStringParser IpcamHttpQueryStringParser;
typedef struct _IpcamHttpQueryStringParserClass IpcamHttpQueryStringParserClass;

struct _IpcamHttpQueryStringParser
{
    GObject parent;
};

struct _IpcamHttpQueryStringParserClass
{
    GObjectClass parent_class;
};

GType ipcam_http_query_string_parser_get_type(void);
GHashTable *ipcam_http_query_string_parser_get(IpcamHttpQueryStringParser *parser,
                                               gchar *query_string);

#endif /* __HTTP_QUERY_STRING_PARSER_H__ */
