#ifndef __HTTP_USERS_HANDLER_H__
#define __HTTP_USERS_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_USERS_HANDLER_TYPE (ipcam_http_users_handler_get_type())
#define IPCAM_HTTP_USERS_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_USERS_HANDLER_TYPE, IpcamHttpUsersHandler))
#define IPCAM_HTTP_USERS_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_USERS_HANDLER_TYPE, IpcamHttpUsersHandlerClass))
#define IPCAM_IS_HTTP_USERS_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_USERS_HANDLER_TYPE))
#define IPCAM_IS_HTTP_USERS_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_USERS_HANDLER_TYPE))
#define IPCAM_HTTP_USERS_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_USERS_HANDLER_TYPE, IpcamHttpUsersHandlerClass))

typedef struct _IpcamHttpUsersHandler IpcamHttpUsersHandler;
typedef struct _IpcamHttpUsersHandlerClass IpcamHttpUsersHandlerClass;

struct _IpcamHttpUsersHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpUsersHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_users_handler_get_type(void);

#endif /* __HTTP_USERS_HANDLER_H__ */
