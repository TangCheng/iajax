#ifndef __HTTP_DATETIME_HANDLER_H__
#define __HTTP_DATETIME_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_DATETIME_HANDLER_TYPE (ipcam_http_datetime_handler_get_type())
#define IPCAM_HTTP_DATETIME_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_DATETIME_HANDLER_TYPE, IpcamHttpDatetimeHandler))
#define IPCAM_HTTP_DATETIME_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_DATETIME_HANDLER_TYPE, IpcamHttpDatetimeHandlerClass))
#define IPCAM_IS_HTTP_DATETIME_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_DATETIME_HANDLER_TYPE))
#define IPCAM_IS_HTTP_DATETIME_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_DATETIME_HANDLER_TYPE))
#define IPCAM_HTTP_DATETIME_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_DATETIME_HANDLER_TYPE, IpcamHttpDatetimeHandlerClass))

typedef struct _IpcamHttpDatetimeHandler IpcamHttpDatetimeHandler;
typedef struct _IpcamHttpDatetimeHandlerClass IpcamHttpDatetimeHandlerClass;

struct _IpcamHttpDatetimeHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpDatetimeHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_datetime_handler_get_type(void);

#endif /* __HTTP_DATETIME_HANDLER_H__ */
