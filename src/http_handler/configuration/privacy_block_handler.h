#ifndef __HTTP_PRIVACY_BLOCK_HANDLER_H__
#define __HTTP_PRIVACY_BLOCK_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_TYPE (ipcam_http_privacy_block_handler_get_type())
#define IPCAM_HTTP_PRIVACY_BLOCK_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_TYPE, IpcamHttpPrivacyBlockHandler))
#define IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_TYPE, IpcamHttpPrivacyBlockHandlerClass))
#define IPCAM_IS_HTTP_PRIVACY_BLOCK_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_TYPE))
#define IPCAM_IS_HTTP_PRIVACY_BLOCK_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_TYPE))
#define IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_TYPE, IpcamHttpPrivacyBlockHandlerClass))

typedef struct _IpcamHttpPrivacyBlockHandler IpcamHttpPrivacyBlockHandler;
typedef struct _IpcamHttpPrivacyBlockHandlerClass IpcamHttpPrivacyBlockHandlerClass;

struct _IpcamHttpPrivacyBlockHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpPrivacyBlockHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_privacy_block_handler_get_type(void);

#endif /* __HTTP_PRIVACY_BLOCK_HANDLER_H__ */
