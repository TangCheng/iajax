#ifndef __LOGIN_MANAGER_H__
#define __LOGIN_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#define IPCAM_LOGIN_MANAGER_TYPE (ipcam_login_manager_get_type())
#define IPCAM_LOGIN_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), IPCAM_LOGIN_MANAGER_TYPE, IpcamLoginManager))
#define IPCAM_LOGIN_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), IPCAM_LOGIN_MANAGER_TYPE, IpcamLoginManagerClass))
#define IPCAM_IS_LOGIN_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), IPCAM_LOGIN_MANAGER_TYPE))
#define IPCAM_IS_LOGIN_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), IPCAM_LOGIN_MANAGER_TYPE))
#define IPCAM_LOGIN_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), IPCAM_LOGIN_MANAGER_TYPE, IpcamLoginManagerClass))

typedef struct _IpcamLoginManager IpcamLoginManager;
typedef struct _IpcamLoginManagerClass IpcamLoginManagerClass;

struct _IpcamLoginManager
{
    GObject parent;
};

struct _IpcamLoginManagerClass
{
    GObjectClass parent_class;
};

GType ipcam_login_manager_get_type(void);
gboolean ipcam_login_manager_login(IpcamLoginManager *login_manager,
                                   const gchar *username,
                                   const gchar *password,
                                   gchar **token,
                                   gchar **role);
gboolean ipcam_login_manager_check_login(IpcamLoginManager *login_manager,
                                         const gchar *token,
                                         gchar **role);
void ipcam_login_manager_logout(IpcamLoginManager *login_manager,
                                const gchar *token);
void ipcam_login_manager_timeout(IpcamLoginManager *login_manager);

#endif /* __LOGIN_MANAGER_H__ */
