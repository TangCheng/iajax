#ifndef __HTTP_SYSINFO_HANDLER_H__
#define __HTTP_SYSINFO_HANDLER_H__

#include "../request_handler.h"

#define IPCAM_HTTP_SYSINFO_HANDLER_TYPE (ipcam_http_sysinfo_handler_get_type())
#define IPCAM_HTTP_SYSINFO_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_SYSINFO_HANDLER_TYPE, IpcamHttpSysinfoHandler))
#define IPCAM_HTTP_SYSINFO_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_SYSINFO_HANDLER_TYPE, IpcamHttpSysinfoHandlerClass))
#define IPCAM_IS_HTTP_SYSINFO_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_SYSINFO_HANDLER_TYPE))
#define IPCAM_IS_HTTP_SYSINFO_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_SYSINFO_HANDLER_TYPE))
#define IPCAM_HTTP_SYSINFO_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_SYSINFO_HANDLER_TYPE, IpcamHttpSysinfoHandlerClass))

typedef struct _IpcamHttpSysinfoHandler IpcamHttpSysinfoHandler;
typedef struct _IpcamHttpSysinfoHandlerClass IpcamHttpSysinfoHandlerClass;

struct _IpcamHttpSysinfoHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpSysinfoHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_sysinfo_handler_get_type(void);

#endif /* __HTTP_SYSINFO_HANDLER_H__ */
