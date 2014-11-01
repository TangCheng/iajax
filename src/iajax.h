#ifndef __IAJAX_H__
#define __IAJAX_H__

#include <gio/gio.h>
#include <json-glib/json-glib.h>
#include <base_app.h>
#include <messages.h>
#include "ajax/http_response.h"

#define IPCAM_IAJAX_TYPE (ipcam_iajax_get_type())
#define IPCAM_IAJAX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_IAJAX_TYPE, IpcamIAjax))
#define IPCAM_IAJAX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_IAJAX_TYPE, IpcamIAjaxClass))
#define IPCAM_IS_IAJAX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_IAJAX_TYPE))
#define IPCAM_IS_IAJAX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_IAJAX_TYPE))
#define IPCAM_IAJAX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_IAJAX_TYPE, IpcamIAjaxClass))

typedef struct _IpcamIAjax IpcamIAjax;
typedef struct _IpcamIAjaxClass IpcamIAjaxClass;

struct _IpcamIAjax
{
    IpcamBaseApp parent;
};

struct _IpcamIAjaxClass
{
    IpcamBaseAppClass parent_class;
};

GType ipcam_iajax_get_type(void);
void ipcam_iajax_property_handler(IpcamIAjax *iajax, const gchar *name, JsonNode *body);
GVariant *ipcam_iajax_get_configuration(IpcamIAjax *iajax, const gchar *name);
void ipcam_iajax_set_configuration(IpcamIAjax *iajax, IpcamRequestMessage *msg,
                                   IpcamHttpResponse *response, GSocket *socket);
GList *ipcam_iajax_get_users(IpcamIAjax *iajax);
gchar *ipcam_iajax_get_user_pwd(IpcamIAjax *iajax, const gchar *name);
gchar *ipcam_iajax_get_user_role(IpcamIAjax *iajax, const gchar *name);

gboolean ipcam_iajax_login(IpcamIAjax *iajax, const gchar *username,
                           const gchar *password, gchar **token, gchar **role);
gboolean ipcam_iajax_check_login(IpcamIAjax *iajax, const gchar *token, gchar **role);
void ipcam_iajax_logout(IpcamIAjax *iajax, const gchar *token);

#endif /* __IAJAX_H__ */
