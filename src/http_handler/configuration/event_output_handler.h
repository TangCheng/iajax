#ifndef __HTTP_EVENT_OUTPUT_HANDLER_H__
#define __HTTP_EVENT_OUTPUT_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_EVENT_OUTPUT_HANDLER_TYPE (ipcam_http_event_output_handler_get_type())
#define IPCAM_HTTP_EVENT_OUTPUT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_EVENT_OUTPUT_HANDLER_TYPE, IpcamHttpEventOutputHandler))
#define IPCAM_HTTP_EVENT_OUTPUT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_EVENT_OUTPUT_HANDLER_TYPE, IpcamHttpEventOutputHandlerClass))
#define IPCAM_IS_HTTP_EVENT_OUTPUT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_EVENT_OUTPUT_HANDLER_TYPE))
#define IPCAM_IS_HTTP_EVENT_OUTPUT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_EVENT_OUTPUT_HANDLER_TYPE))
#define IPCAM_HTTP_EVENT_OUTPUT_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_EVENT_OUTPUT_HANDLER_TYPE, IpcamHttpEventOutputHandlerClass))

typedef struct _IpcamHttpEventOutputHandler IpcamHttpEventOutputHandler;
typedef struct _IpcamHttpEventOutputHandlerClass IpcamHttpEventOutputHandlerClass;

struct _IpcamHttpEventOutputHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpEventOutputHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_event_output_handler_get_type(void);

#endif /* __HTTP_EVENT_OUTPUT_HANDLER_H__ */
