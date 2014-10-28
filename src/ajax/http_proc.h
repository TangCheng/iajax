#ifndef __HTTP_PROC_H__
#define __HTTP_PROC_H__

#include <gio/gio.h>
#include "http_request.h"
#include "http_response.h"

#define IPCAM_HTTP_PROC_TYPE (ipcam_http_proc_get_type())
#define IPCAM_HTTP_PROC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_HTTP_PROC_TYPE, IpcamHttpProc))
#define IPCAM_HTTP_PROC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_HTTP_PROC_TYPE, IpcamHttpProcClass))
#define IPCAM_IS_HTTP_PROC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_HTTP_PROC_TYPE))
#define IPCAM_IS_HTTP_PROC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_HTTP_PROC_TYPE))
#define IPCAM_HTTP_PROC_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_HTTP_PROC_TYPE, IpcamHttpProcClass))

typedef struct _IpcamHttpProc IpcamHttpProc;
typedef struct _IpcamHttpProcClass IpcamHttpProcClass;

struct _IpcamHttpProc
{
    GObject parent;
};

struct _IpcamHttpProcClass
{
    GObjectClass parent_class;
};

GType ipcam_http_proc_get_type(void);
IpcamHttpResponse *ipcam_http_proc_get_response(IpcamHttpProc *http_proc, IpcamHttpRequest *http_request, GSocket *socket);

#endif /* __HTTP_PROC_H__ */
