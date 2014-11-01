#ifndef __HTTP_LOGOUT_HANDLER_H__
#define __HTTP_LOGOUT_HANDLER_H__

#include "../request_handler.h"

#define IPCAM_HTTP_LOGOUT_HANDLER_TYPE (ipcam_http_logout_handler_get_type())
#define IPCAM_HTTP_LOGOUT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_LOGOUT_HANDLER_TYPE, IpcamHttpLogoutHandler))
#define IPCAM_HTTP_LOGOUT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_LOGOUT_HANDLER_TYPE, IpcamHttpLogoutHandlerClass))
#define IPCAM_IS_HTTP_LOGOUT_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_LOGOUT_HANDLER_TYPE))
#define IPCAM_IS_HTTP_LOGOUT_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_LOGOUT_HANDLER_TYPE))
#define IPCAM_HTTP_LOGOUT_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_LOGOUT_HANDLER_TYPE, IpcamHttpLogoutHandlerClass))

typedef struct _IpcamHttpLogoutHandler IpcamHttpLogoutHandler;
typedef struct _IpcamHttpLogoutHandlerClass IpcamHttpLogoutHandlerClass;

struct _IpcamHttpLogoutHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpLogoutHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_logout_handler_get_type(void);

#endif /* __HTTP_LOGOUT_HANDLER_H__ */
