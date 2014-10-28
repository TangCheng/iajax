#ifndef __HTTP_IMAGE_HANDLER_H__
#define __HTTP_IMAGE_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_IMAGE_HANDLER_TYPE (ipcam_http_image_handler_get_type())
#define IPCAM_HTTP_IMAGE_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_IMAGE_HANDLER_TYPE, IpcamHttpImageHandler))
#define IPCAM_HTTP_IMAGE_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_IMAGE_HANDLER_TYPE, IpcamHttpImageHandlerClass))
#define IPCAM_IS_HTTP_IMAGE_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_IMAGE_HANDLER_TYPE))
#define IPCAM_IS_HTTP_IMAGE_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_IMAGE_HANDLER_TYPE))
#define IPCAM_HTTP_IMAGE_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_IMAGE_HANDLER_TYPE, IpcamHttpImageHandlerClass))

typedef struct _IpcamHttpImageHandler IpcamHttpImageHandler;
typedef struct _IpcamHttpImageHandlerClass IpcamHttpImageHandlerClass;

struct _IpcamHttpImageHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpImageHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_image_handler_get_type(void);

#endif /* __HTTP_IMAGE_HANDLER_H__ */
