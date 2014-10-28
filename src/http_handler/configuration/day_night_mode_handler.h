#ifndef __HTTP_DAY_NIGHT_MODE_HANDLER_H__
#define __HTTP_DAY_NIGHT_MODE_HANDLER_H__

#include "http_request_handler.h"

#define IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_TYPE (ipcam_http_day_night_mode_handler_get_type())
#define IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_TYPE, IpcamHttpDayNightModeHandler))
#define IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_TYPE, IpcamHttpDayNightModeHandlerClass))
#define IPCAM_IS_HTTP_DAY_NIGHT_MODE_HANDLER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_TYPE))
#define IPCAM_IS_HTTP_DAY_NIGHT_MODE_HANDLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_TYPE))
#define IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_TYPE, IpcamHttpDayNightModeHandlerClass))

typedef struct _IpcamHttpDayNightModeHandler IpcamHttpDayNightModeHandler;
typedef struct _IpcamHttpDayNightModeHandlerClass IpcamHttpDayNightModeHandlerClass;

struct _IpcamHttpDayNightModeHandler
{
    IpcamHttpRequestHandler parent;
};

struct _IpcamHttpDayNightModeHandlerClass
{
    IpcamHttpRequestHandlerClass parent_class;
};

GType ipcam_http_day_night_mode_handler_get_type(void);

#endif /* __HTTP_DAY_NIGHT_MODE_HANDLER_H__ */
