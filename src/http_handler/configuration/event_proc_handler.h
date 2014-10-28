#ifndef __HTTP_EVENT_PROC_HANDLER_H__
#define __HTTP_EVENT_PROC_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_EVENT_PROC_HANDLER_TYPE (ipcam_http_event_proc_handler_get_type())
#define IPCAM_HTTP_EVENT_PROC_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_EVENT_PROC_HANDLER_TYPE, IpcamHttpEventProcHandler))
#define IPCAM_HTTP_EVENT_PROC_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_EVENT_PROC_HANDLER_TYPE, IpcamHttpEventProcHandlerClass))
#define IPCAM_IS_HTTP_EVENT_PROC_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_EVENT_PROC_HANDLER_TYPE))
#define IPCAM_IS_HTTP_EVENT_PROC_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_EVENT_PROC_HANDLER_TYPE))
#define IPCAM_HTTP_EVENT_PROC_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_EVENT_PROC_HANDLER_TYPE, IpcamHttpEventProcHandlerClass))

typedef struct _IpcamHttpEventProcHandler IpcamHttpEventProcHandler;
typedef struct _IpcamHttpEventProcHandlerClass IpcamHttpEventProcHandlerClass;

struct _IpcamHttpEventProcHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpEventProcHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_event_proc_handler_get_type(void);

#endif /* __HTTP_EVENT_PROC_HANDLER_H__ */
