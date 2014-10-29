#ifndef __HTTP_REQUEST_HANDLER_H__
#define __HTTP_REQUEST_HANDLER_H__

#include <gio/gio.h>
#include <http_parser.h>
#include <json-glib/json-glib.h>
#include "ajax/http_request.h"
#include "ajax/http_response.h"

#define IPCAM_HTTP_REQUEST_HANDLER_TYPE (ipcam_http_request_handler_get_type())
#define IPCAM_HTTP_REQUEST_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_REQUEST_HANDLER_TYPE, IpcamHttpRequestHandler))
#define IPCAM_HTTP_REQUEST_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_REQUEST_HANDLER_TYPE, IpcamHttpRequestHandlerClass))
#define IPCAM_IS_HTTP_REQUEST_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_REQUEST_HANDLER_TYPE))
#define IPCAM_IS_HTTP_REQUEST_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_REQUEST_HANDLER_TYPE))
#define IPCAM_HTTP_REQUEST_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_REQUEST_HANDLER_TYPE, IpcamHttpRequestHandlerClass))

typedef struct _IpcamHttpRequestHandler IpcamHttpRequestHandler;
typedef struct _IpcamHttpRequestHandlerClass IpcamHttpRequestHandlerClass;

struct _IpcamHttpRequestHandler
{
    GObject parent;
};

struct _IpcamHttpRequestHandlerClass
{
    GObjectClass parent_class;
};

typedef struct _IpcamHttpRequestHandlerData
{
    gboolean (*func)(IpcamHttpRequestHandler *, IpcamHttpRequest *, IpcamHttpResponse *, GSocket *);
    enum http_method method;
    gchar *path;
} IpcamHttpRequestHandlerData;
typedef IpcamHttpRequestHandlerData handler_data;

#define START_HANDLER(NAME, METHOD, PATH, REQUEST, RESULT, SOCKET)             \
static gboolean ipcam_http_##NAME##_func(IpcamHttpRequestHandler *, IpcamHttpRequest *, IpcamHttpResponse *, GSocket *); \
handler_data NAME##_data = {ipcam_http_##NAME##_func, METHOD, PATH};  \
handler_data *NAME = &NAME##_data; \
static gboolean ipcam_http_##NAME##_func(IpcamHttpRequestHandler *NAME, IpcamHttpRequest *REQUEST, IpcamHttpResponse *RESULT, GSocket *SOCKET) { \
    gboolean ret = FALSE;


#define END_HANDLER \
    return ret; \
}

GType ipcam_http_request_handler_get_type(void);
gboolean ipcam_http_request_handler_dispatch(IpcamHttpRequestHandler *http_request_handler,
                                             IpcamHttpRequest *http_request,
                                             IpcamHttpResponse *http_response,
                                             GSocket *socket);
void ipcam_http_request_handler_register(IpcamHttpRequestHandler *http_request_handler,
                                         handler_data *handler);
void ipcam_http_request_handler_do_put_action(IpcamHttpRequestHandler *http_request_handler,
                                              IpcamHttpRequest *request, IpcamHttpResponse *response,
                                              GSocket *socket, const gchar *action);
void add_value(JsonBuilder *builder, const gchar *name, GVariant *value);

#endif /* __HTTP_REQUEST_HANDLER_H__ */
