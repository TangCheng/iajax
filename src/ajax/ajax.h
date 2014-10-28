#ifndef __AJAX_H__
#define __AJAX_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_AJAX_TYPE (ipcam_ajax_get_type())
#define IPCAM_AJAX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_AJAX_TYPE, IpcamAjax))
#define IPCAM_AJAX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_AJAX_TYPE, IpcamAjaxClass))
#define IPCAM_IS_AJAX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_AJAX_TYPE))
#define IPCAM_IS_AJAX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_AJAX_TYPE))
#define IPCAM_AJAX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_AJAX_TYPE, IpcamAjaxClass))

typedef struct _IpcamAjax IpcamAjax;
typedef struct _IpcamAjaxClass IpcamAjaxClass;

struct _IpcamAjax
{
    GObject parent;
};

struct _IpcamAjaxClass
{
    GObjectClass parent_class;
};

GType ipcam_ajax_get_type(void);
gboolean ipcam_ajax_get_terminated(IpcamAjax *ajax);

#endif /* __AJAX_H__ */
