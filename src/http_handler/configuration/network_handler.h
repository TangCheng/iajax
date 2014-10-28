#ifndef __HTTP_NETWORK_HANDLER_H__
#define __HTTP_NETWORK_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_NETWORK_HANDLER_TYPE (ipcam_http_network_handler_get_type())
#define IPCAM_HTTP_NETWORK_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_NETWORK_HANDLER_TYPE, IpcamHttpNetworkHandler))
#define IPCAM_HTTP_NETWORK_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_NETWORK_HANDLER_TYPE, IpcamHttpNetworkHandlerClass))
#define IPCAM_IS_HTTP_NETWORK_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_NETWORK_HANDLER_TYPE))
#define IPCAM_IS_HTTP_NETWORK_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_NETWORK_HANDLER_TYPE))
#define IPCAM_HTTP_NETWORK_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_NETWORK_HANDLER_TYPE, IpcamHttpNetworkHandlerClass))

typedef struct _IpcamHttpNetworkHandler IpcamHttpNetworkHandler;
typedef struct _IpcamHttpNetworkHandlerClass IpcamHttpNetworkHandlerClass;

struct _IpcamHttpNetworkHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpNetworkHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_network_handler_get_type(void);

#endif /* __HTTP_NETWORK_HANDLER_H__ */
