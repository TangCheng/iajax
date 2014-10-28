#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

#include "http_request.h"

#define IPCAM_HTTP_PARSER_TYPE (ipcam_http_parser_get_type())
#define IPCAM_HTTP_PARSER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_PARSER_TYPE, IpcamHttpParser))
#define IPCAM_HTTP_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_PARSER_TYPE, IpcamHttpParserClass))
#define IPCAM_IS_HTTP_PARSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_PARSER_TYPE))
#define IPCAM_IS_HTTP_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_PARSER_TYPE))
#define IPCAM_HTTP_PARSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_PARSER_TYPE, IpcamHttpParserClass))

typedef struct _IpcamHttpParser IpcamHttpParser;
typedef struct _IpcamHttpParserClass IpcamHttpParserClass;

struct _IpcamHttpParser
{
    GObject parent;
};

struct _IpcamHttpParserClass
{
    GObjectClass parent_class;
};

GType ipcam_http_parser_get_type(void);
IpcamHttpRequest *ipcam_http_parser_get_request(IpcamHttpParser *self, gchar *http_buffer, gssize len);

#endif /* __HTTP_PARSER_H__ */
