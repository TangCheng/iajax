#ifndef __HTTP_BASE_INFO_HANDLER_H__
#define __HTTP_BASE_INFO_HANDLER_H__

#include "../request_handler.h"

#define IPCAM_HTTP_BASE_INFO_HANDLER_TYPE (ipcam_http_base_info_handler_get_type())
#define IPCAM_HTTP_BASE_INFO_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_BASE_INFO_HANDLER_TYPE, IpcamHttpBaseInfoHandler))
#define IPCAM_HTTP_BASE_INFO_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_BASE_INFO_HANDLER_TYPE, IpcamHttpBaseInfoHandlerClass))
#define IPCAM_IS_HTTP_BASE_INFO_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_BASE_INFO_HANDLER_TYPE))
#define IPCAM_IS_HTTP_BASE_INFO_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_BASE_INFO_HANDLER_TYPE))
#define IPCAM_HTTP_BASE_INFO_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_BASE_INFO_HANDLER_TYPE, IpcamHttpBaseInfoHandlerClass))

typedef struct _IpcamHttpBaseInfoHandler IpcamHttpBaseInfoHandler;
typedef struct _IpcamHttpBaseInfoHandlerClass IpcamHttpBaseInfoHandlerClass;

struct _IpcamHttpBaseInfoHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpBaseInfoHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_base_info_handler_get_type(void);

#endif /* __HTTP_BASE_INFO_HANDLER_H__ */
