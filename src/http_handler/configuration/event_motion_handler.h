#ifndef __HTTP_EVENT_MOTION_HANDLER_H__
#define __HTTP_EVENT_MOTION_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_EVENT_MOTION_HANDLER_TYPE (ipcam_http_event_motion_handler_get_type())
#define IPCAM_HTTP_EVENT_MOTION_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_EVENT_MOTION_HANDLER_TYPE, IpcamHttpEventMotionHandler))
#define IPCAM_HTTP_EVENT_MOTION_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_EVENT_MOTION_HANDLER_TYPE, IpcamHttpEventMotionHandlerClass))
#define IPCAM_IS_HTTP_EVENT_MOTION_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_EVENT_MOTION_HANDLER_TYPE))
#define IPCAM_IS_HTTP_EVENT_MOTION_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_EVENT_MOTION_HANDLER_TYPE))
#define IPCAM_HTTP_EVENT_MOTION_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_EVENT_MOTION_HANDLER_TYPE, IpcamHttpEventMotionHandlerClass))

typedef struct _IpcamHttpEventMotionHandler IpcamHttpEventMotionHandler;
typedef struct _IpcamHttpEventMotionHandlerClass IpcamHttpEventMotionHandlerClass;

struct _IpcamHttpEventMotionHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpEventMotionHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_event_motion_handler_get_type(void);

#endif /* __HTTP_EVENT_MOTION_HANDLER_H__ */
