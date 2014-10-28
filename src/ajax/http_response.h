#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#define IPCAM_HTTP_RESPONSE_TYPE (ipcam_http_response_get_type())
#define IPCAM_HTTP_RESPONSE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_RESPONSE_TYPE, IpcamHttpResponse))
#define IPCAM_HTTP_RESPONSE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_RESPONSE_TYPE, IpcamHttpResponseClass))
#define IPCAM_IS_HTTP_RESPONSE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_RESPONSE_TYPE))
#define IPCAM_IS_HTTP_RESPONSE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_RESPONSE_TYPE))
#define IPCAM_HTTP_RESPONSE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_RESPONSE_TYPE, IpcamHttpResponseClass))

typedef struct _IpcamHttpResponse IpcamHttpResponse;
typedef struct _IpcamHttpResponseClass IpcamHttpResponseClass;

struct _IpcamHttpResponse
{
    GObject parent;
};

struct _IpcamHttpResponseClass
{
    GObjectClass parent_class;
};

GType ipcam_http_response_get_type(void);
void ipcam_http_response_success(IpcamHttpResponse *http_response, gboolean success);
void ipcam_http_response_write_string(IpcamHttpResponse *http_response, GSocket *socket);

#endif /* __HTTP_RESPONSE_H__ */
