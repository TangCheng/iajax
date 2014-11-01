#define _GNU_SOURCE
#include <stdio.h>
#include <czmq.h>
#include "login_manager.h"
#include "sha1.h"
#include "iajax.h"

#define LOGIN_TIMEOUT   (10 * 60 / 30) /* unit is second, but we should call timeout every 30 secs */

enum {
  PROP_0,
  PROP_APP,
  N_PROPERTIES
};

typedef struct _IpcamLoginManagerHashValue
{
    gchar *uuid;
    gchar *username;
    gchar *token;
    guint timeout;
} IpcamLoginManagerHashValue;

typedef IpcamLoginManagerHashValue hash_value;

typedef struct _IpcamLoginManagerPrivate
{
    IpcamIAjax *iajax;
    GHashTable *logins_hash;
    GMutex mutex;
} IpcamLoginManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamLoginManager, ipcam_login_manager, G_TYPE_OBJECT);

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_login_manager_dispose(GObject *self)
{
    static gboolean first_run = TRUE;

    if (first_run)
    {
        first_run = FALSE;
        G_OBJECT_CLASS(ipcam_login_manager_parent_class)->dispose(self);
    }
}
static void ipcam_login_manager_finalize(GObject *self)
{
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(IPCAM_LOGIN_MANAGER(self));
    g_mutex_lock(&priv->mutex);
    g_hash_table_destroy(priv->logins_hash);
    g_mutex_unlock(&priv->mutex);
    g_mutex_clear(&priv->mutex);
    G_OBJECT_CLASS(ipcam_login_manager_parent_class)->finalize(self);
}
static void destroy(gpointer data)
{
    hash_value *value = (hash_value *)data;
    g_free(value->uuid);
    g_free(value->username);
    g_free(value->token);
    g_free(value);
}
static void ipcam_login_manager_init(IpcamLoginManager *self)
{
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(self);
    g_mutex_init(&priv->mutex);
    priv->logins_hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)destroy);
}
static void ipcam_login_manager_get_property(GObject    *object,
                                             guint       property_id,
                                             GValue     *value,
                                             GParamSpec *pspec)
{
    IpcamLoginManager *self = IPCAM_LOGIN_MANAGER(object);
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(self);
    switch(property_id)
    {
    case PROP_APP:
        {
            g_value_set_object(value, priv->iajax);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_login_manager_set_property(GObject      *object,
                                             guint         property_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
{
    IpcamLoginManager *self = IPCAM_LOGIN_MANAGER(object);
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(self);
    switch(property_id)
    {
    case PROP_APP:
        {
            priv->iajax = g_value_get_object(value);
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void ipcam_login_manager_class_init(IpcamLoginManagerClass *klass)
{
    GObjectClass *this_class = G_OBJECT_CLASS(klass);
    this_class->dispose = &ipcam_login_manager_dispose;
    this_class->finalize = &ipcam_login_manager_finalize;
    this_class->set_property = &ipcam_login_manager_set_property;
    this_class->get_property = &ipcam_login_manager_get_property;
    
    obj_properties[PROP_APP] =
        g_param_spec_object("app",
                            "Application",
                            "The parent application of the ajax thread.",
                            IPCAM_IAJAX_TYPE, // default value
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties(this_class, N_PROPERTIES, obj_properties);
}

gboolean ipcam_login_manager_login(IpcamLoginManager *login_manager,
                                   const gchar *username,
                                   const gchar *password,
                                   gchar **token,
                                   gchar **role)
{
    g_return_val_if_fail(IPCAM_IS_LOGIN_MANAGER(login_manager), FALSE);
    gboolean ret = FALSE;
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(login_manager);
    zuuid_t *generator = NULL;
    gchar *pwd = NULL;
    gchar sha1v[SHA1_DIGEST_SIZE];
    gchar local_pwd[SHA1_DIGEST_SIZE * 2 + 1];
    gchar temp[3];
    hash_value *value = NULL;
    gint i = 0;

    pwd = ipcam_iajax_get_user_pwd(priv->iajax, username);
    if (pwd)
    {
        sha1(sha1v, pwd, strlen(pwd));
        memset(local_pwd, 0, SHA1_DIGEST_SIZE * 2 + 1);
        temp[2] = '\0';
        for (i = 0; i < SHA1_DIGEST_SIZE; i++)
        {
            g_snprintf(temp, 3, "%02x", (guchar)sha1v[i]);
            g_strlcat(local_pwd, temp, SHA1_DIGEST_SIZE * 2 + 1);
        }

        if (g_str_equal(password, local_pwd))
        {
            value = g_new(hash_value, 1);

            generator = zuuid_new();
            value->uuid = g_strdup(zuuid_str(generator));
            zuuid_destroy(&generator);

            value->username = g_strdup(username);
            sha1(sha1v, value->uuid, strlen(value->uuid));
            value->token = g_base64_encode(sha1v, SHA1_DIGEST_SIZE);
            value->timeout = 0;
            
            *role = ipcam_iajax_get_user_role(priv->iajax, value->username);
            *token = g_strdup(value->token);
            
            g_mutex_lock(&priv->mutex);
            g_hash_table_insert(priv->logins_hash, g_strdup(value->token), value);
            g_mutex_unlock(&priv->mutex);
            ret = TRUE;
        }
        
        g_free(pwd);
    }

    return ret;
}

gboolean ipcam_login_manager_check_login(IpcamLoginManager *login_manager,
                                         const gchar *token,
                                         gchar **role)
{
    g_return_val_if_fail(IPCAM_IS_LOGIN_MANAGER(login_manager), FALSE);
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(login_manager);
    gboolean ret = FALSE;
    hash_value *value;

    g_mutex_lock(&priv->mutex);
    value = g_hash_table_lookup(priv->logins_hash, token);
    if (value)
    {
        *role = ipcam_iajax_get_user_role(priv->iajax, value->username);
        value->timeout = 0;
        ret = TRUE;
    }
    g_mutex_unlock(&priv->mutex);
    
    return ret;
}

void ipcam_login_manager_logout(IpcamLoginManager *login_manager,
                                const gchar *token)
{
    g_return_if_fail(IPCAM_IS_LOGIN_MANAGER(login_manager));
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(login_manager);
    g_mutex_lock(&priv->mutex);
    g_hash_table_remove(priv->logins_hash, token);
    g_mutex_unlock(&priv->mutex);
}
 
static gboolean login_timeout(gpointer key, gpointer value, gpointer data)
{
    hash_value *login = (hash_value *)value;
    login->timeout++;
    if (login->timeout > LOGIN_TIMEOUT)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void ipcam_login_manager_timeout(IpcamLoginManager *login_manager)
{
    g_return_if_fail(IPCAM_IS_LOGIN_MANAGER(login_manager));
    IpcamLoginManagerPrivate *priv = ipcam_login_manager_get_instance_private(login_manager);
    g_mutex_lock(&priv->mutex);
    g_hash_table_foreach_remove(priv->logins_hash, login_timeout, login_manager);
    g_mutex_unlock(&priv->mutex);
}

