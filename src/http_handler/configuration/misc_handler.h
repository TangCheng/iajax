#ifndef __HTTP_MISC_HANDLER_H__
#define __HTTP_MISC_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_MISC_HANDLER_TYPE (ipcam_http_misc_handler_get_type())
#define IPCAM_HTTP_MISC_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_MISC_HANDLER_TYPE, IpcamHttpMiscHandler))
#define IPCAM_HTTP_MISC_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_MISC_HANDLER_TYPE, IpcamHttpMiscHandlerClass))
#define IPCAM_IS_HTTP_MISC_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_MISC_HANDLER_TYPE))
#define IPCAM_IS_HTTP_MISC_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_MISC_HANDLER_TYPE))
#define IPCAM_HTTP_MISC_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_MISC_HANDLER_TYPE, IpcamHttpMiscHandlerClass))

typedef struct _IpcamHttpMiscHandler IpcamHttpMiscHandler;
typedef struct _IpcamHttpMiscHandlerClass IpcamHttpMiscHandlerClass;

struct _IpcamHttpMiscHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpMiscHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_misc_handler_get_type(void);

#endif /* __HTTP_MISC_HANDLER_H__ */
