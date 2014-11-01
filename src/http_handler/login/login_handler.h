#ifndef __HTTP_LOGIN_HANDLER_H__
#define __HTTP_LOGIN_HANDLER_H__

#include "../request_handler.h"

#define IPCAM_HTTP_LOGIN_HANDLER_TYPE (ipcam_http_login_handler_get_type())
#define IPCAM_HTTP_LOGIN_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_LOGIN_HANDLER_TYPE, IpcamHttpLoginHandler))
#define IPCAM_HTTP_LOGIN_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_LOGIN_HANDLER_TYPE, IpcamHttpLoginHandlerClass))
#define IPCAM_IS_HTTP_LOGIN_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_LOGIN_HANDLER_TYPE))
#define IPCAM_IS_HTTP_LOGIN_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_LOGIN_HANDLER_TYPE))
#define IPCAM_HTTP_LOGIN_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_LOGIN_HANDLER_TYPE, IpcamHttpLoginHandlerClass))

typedef struct _IpcamHttpLoginHandler IpcamHttpLoginHandler;
typedef struct _IpcamHttpLoginHandlerClass IpcamHttpLoginHandlerClass;

struct _IpcamHttpLoginHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpLoginHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_login_handler_get_type(void);

#endif /* __HTTP_LOGIN_HANDLER_H__ */
