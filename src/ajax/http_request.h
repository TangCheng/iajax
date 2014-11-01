#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <glib.h>
#include <glib-object.h>
#include "http_response.h"

#define IPCAM_HTTP_REQUEST_TYPE (ipcam_http_request_get_type())
#define IPCAM_HTTP_REQUEST(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_REQUEST_TYPE, IpcamHttpRequest))
#define IPCAM_HTTP_REQUEST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_REQUEST_TYPE, IpcamHttpRequestClass))
#define IPCAM_IS_HTTP_REQUEST(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_REQUEST_TYPE))
#define IPCAM_IS_HTTP_REQUEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_REQUEST_TYPE))
#define IPCAM_HTTP_REQUEST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_REQUEST_TYPE, IpcamHttpRequestClass))

typedef struct _IpcamHttpRequest IpcamHttpRequest;
typedef struct _IpcamHttpRequestClass IpcamHttpRequestClass;

struct _IpcamHttpRequest
{
    GObject parent;
};

struct _IpcamHttpRequestClass
{
    GObjectClass parent_class;
};

GType ipcam_http_request_get_type(void);
void ipcam_http_request_add_header_field(IpcamHttpRequest *http_request, gchar *field);
void ipcam_http_request_add_header_value(IpcamHttpRequest *http_request, gchar *value);
gchar *ipcam_http_request_get_header_value(IpcamHttpRequest *http_request, gchar *field);
gboolean ipcam_http_request_is_get(IpcamHttpRequest *http_request);
IpcamHttpResponse *ipcam_http_request_get_response(IpcamHttpRequest *http_request, guint code);

#endif /* __HTTP_REQUEST_H__ */
