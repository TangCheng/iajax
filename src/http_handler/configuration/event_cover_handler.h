#ifndef __HTTP_EVENT_COVER_HANDLER_H__
#define __HTTP_EVENT_COVER_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_EVENT_COVER_HANDLER_TYPE (ipcam_http_event_cover_handler_get_type())
#define IPCAM_HTTP_EVENT_COVER_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_EVENT_COVER_HANDLER_TYPE, IpcamHttpEventCoverHandler))
#define IPCAM_HTTP_EVENT_COVER_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_EVENT_COVER_HANDLER_TYPE, IpcamHttpEventCoverHandlerClass))
#define IPCAM_IS_HTTP_EVENT_COVER_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_EVENT_COVER_HANDLER_TYPE))
#define IPCAM_IS_HTTP_EVENT_COVER_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_EVENT_COVER_HANDLER_TYPE))
#define IPCAM_HTTP_EVENT_COVER_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_EVENT_COVER_HANDLER_TYPE, IpcamHttpEventCoverHandlerClass))

typedef struct _IpcamHttpEventCoverHandler IpcamHttpEventCoverHandler;
typedef struct _IpcamHttpEventCoverHandlerClass IpcamHttpEventCoverHandlerClass;

struct _IpcamHttpEventCoverHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpEventCoverHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_event_cover_handler_get_type(void);

#endif /* __HTTP_EVENT_COVER_HANDLER_H__ */
