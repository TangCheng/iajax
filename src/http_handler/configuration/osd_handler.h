#ifndef __HTTP_OSD_HANDLER_H__
#define __HTTP_OSD_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_OSD_HANDLER_TYPE (ipcam_http_osd_handler_get_type())
#define IPCAM_HTTP_OSD_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_OSD_HANDLER_TYPE, IpcamHttpOsdHandler))
#define IPCAM_HTTP_OSD_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_OSD_HANDLER_TYPE, IpcamHttpOsdHandlerClass))
#define IPCAM_IS_HTTP_OSD_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_OSD_HANDLER_TYPE))
#define IPCAM_IS_HTTP_OSD_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_OSD_HANDLER_TYPE))
#define IPCAM_HTTP_OSD_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_OSD_HANDLER_TYPE, IpcamHttpOsdHandlerClass))

typedef struct _IpcamHttpOsdHandler IpcamHttpOsdHandler;
typedef struct _IpcamHttpOsdHandlerClass IpcamHttpOsdHandlerClass;

struct _IpcamHttpOsdHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpOsdHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_osd_handler_get_type(void);

#endif /* __HTTP_OSD_HANDLER_H__ */
