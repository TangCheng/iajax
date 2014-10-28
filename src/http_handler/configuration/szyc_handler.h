#ifndef __HTTP_SZYC_HANDLER_H__
#define __HTTP_SZYC_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_SZYC_HANDLER_TYPE (ipcam_http_szyc_handler_get_type())
#define IPCAM_HTTP_SZYC_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_SZYC_HANDLER_TYPE, IpcamHttpSzycHandler))
#define IPCAM_HTTP_SZYC_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_SZYC_HANDLER_TYPE, IpcamHttpSzycHandlerClass))
#define IPCAM_IS_HTTP_SZYC_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_SZYC_HANDLER_TYPE))
#define IPCAM_IS_HTTP_SZYC_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_SZYC_HANDLER_TYPE))
#define IPCAM_HTTP_SZYC_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_SZYC_HANDLER_TYPE, IpcamHttpSzycHandlerClass))

typedef struct _IpcamHttpSzycHandler IpcamHttpSzycHandler;
typedef struct _IpcamHttpSzycHandlerClass IpcamHttpSzycHandlerClass;

struct _IpcamHttpSzycHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpSzycHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_szyc_handler_get_type(void);

#endif /* __HTTP_SZYC_HANDLER_H__ */
