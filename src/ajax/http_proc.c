#include <stdio.h>
#include <string.h>
#include "http_proc.h"
#include "http_handler/request_handler.h"
#include "http_handler/configuration/misc_handler.h"
#include "http_handler/configuration/base_info_handler.h"
#include "http_handler/configuration/users_handler.h"
#include "http_handler/configuration/datetime_handler.h"
#include "http_handler/configuration/video_handler.h"
#include "http_handler/configuration/image_handler.h"
#include "http_handler/configuration/privacy_block_handler.h"
#include "http_handler/configuration/day_night_mode_handler.h"
#include "http_handler/configuration/osd_handler.h"
#include "http_handler/configuration/szyc_handler.h"
#include "http_handler/configuration/network_handler.h"
#include "http_handler/configuration/event_input_handler.h"
#include "http_handler/configuration/event_output_handler.h"
#include "http_handler/configuration/event_motion_handler.h"
#include "http_handler/configuration/event_cover_handler.h"
#include "http_handler/configuration/event_proc_handler.h"

#include "http_handler/login/login_handler.h"
#include "http_handler/login/logout_handler.h"

#include "http_handler/upgrade/upgrade_handler.h"

#include "http_handler/system/sysinfo_handler.h"
#include "http_handler/system/system_handler.h"

#include "iajax.h"
#include "common.h"

enum
{
    PROP_0,
    PROP_APP,
    N_PROPERTIES
};

typedef struct _IpcamHttpProcPrivate
{
    IpcamIAjax *iajax;
    GList *request_handler_list;
} IpcamHttpProcPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamHttpProc, ipcam_http_proc, G_TYPE_OBJECT)

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void ipcam_http_proc_error(IpcamHttpProc *http_proc,
                                  IpcamHttpRequest *http_request,
                                  IpcamHttpResponse *http_response);

static void request_handler_destroy_func(gpointer data)
{
    g_object_unref(G_OBJECT(data));
}

static void ipcam_http_proc_finalize(GObject *object)
{
    IpcamHttpProcPrivate *priv = ipcam_http_proc_get_instance_private(IPCAM_HTTP_PROC(object));

    g_list_free_full(priv->request_handler_list, request_handler_destroy_func);
    G_OBJECT_CLASS(ipcam_http_proc_parent_class)->finalize(object);
}
static void ipcam_http_proc_init(IpcamHttpProc *self)
{
    IpcamHttpProcPrivate *priv = ipcam_http_proc_get_instance_private(self);
    IpcamHttpRequestHandler *req_handler;

    GType http_handler_type[] =
    {
        IPCAM_HTTP_MISC_HANDLER_TYPE,
        IPCAM_HTTP_BASE_INFO_HANDLER_TYPE,
        IPCAM_HTTP_USERS_HANDLER_TYPE,
        IPCAM_HTTP_DATETIME_HANDLER_TYPE,
        IPCAM_HTTP_VIDEO_HANDLER_TYPE,
        IPCAM_HTTP_IMAGE_HANDLER_TYPE,
        IPCAM_HTTP_PRIVACY_BLOCK_HANDLER_TYPE,
        IPCAM_HTTP_DAY_NIGHT_MODE_HANDLER_TYPE,
        IPCAM_HTTP_OSD_HANDLER_TYPE,
        IPCAM_HTTP_SZYC_HANDLER_TYPE,
        IPCAM_HTTP_NETWORK_HANDLER_TYPE,
        IPCAM_HTTP_EVENT_INPUT_HANDLER_TYPE,
        IPCAM_HTTP_EVENT_OUTPUT_HANDLER_TYPE,
        IPCAM_HTTP_EVENT_MOTION_HANDLER_TYPE,
        IPCAM_HTTP_EVENT_COVER_HANDLER_TYPE,
        IPCAM_HTTP_EVENT_PROC_HANDLER_TYPE,

        IPCAM_HTTP_LOGIN_HANDLER_TYPE,
        IPCAM_HTTP_LOGOUT_HANDLER_TYPE,

        IPCAM_HTTP_UPGRADE_HANDLER_TYPE,

        IPCAM_HTTP_SYSINFO_HANDLER_TYPE,
        IPCAM_HTTP_SYSTEM_HANDLER_TYPE,
    };
    gint i = 0;

    for (i = 0; i < ARRAY_SIZE(http_handler_type); i++)
    {
        req_handler = g_object_new(http_handler_type[i], "app", priv->iajax, NULL);
        priv->request_handler_list = g_list_append(priv->request_handler_list, req_handler);
    }

}
static void ipcam_http_proc_get_property(GObject    *object,
                                         guint       property_id,
                                         GValue     *value,
                                         GParamSpec *pspec)
{
    IpcamHttpProc *self = IPCAM_HTTP_PROC(object);
    IpcamHttpProcPrivate *priv = ipcam_http_proc_get_instance_private(self);
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
static void ipcam_http_proc_set_property(GObject      *object,
                                         guint         property_id,
                                         const GValue *value,
                                         GParamSpec   *pspec)
{
    IpcamHttpProc *self = IPCAM_HTTP_PROC(object);
    IpcamHttpProcPrivate *priv = ipcam_http_proc_get_instance_private(self);
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
static void ipcam_http_proc_class_init(IpcamHttpProcClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = &ipcam_http_proc_finalize;
    object_class->get_property = &ipcam_http_proc_get_property;
    object_class->set_property = &ipcam_http_proc_set_property;

    obj_properties[PROP_APP] =
        g_param_spec_object("app",
                            "application",
                            "application.",
                            IPCAM_IAJAX_TYPE, // default value
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}
IpcamHttpResponse *ipcam_http_proc_get_response(IpcamHttpProc *http_proc, IpcamHttpRequest *http_request, GSocket *socket)
{
    g_return_val_if_fail(IPCAM_IS_HTTP_PROC(http_proc), NULL);
    g_return_val_if_fail(IPCAM_IS_HTTP_REQUEST(http_request), NULL);

    guint major, minor;
    IpcamHttpResponse *response = g_object_new(IPCAM_HTTP_RESPONSE_TYPE, NULL);
    g_object_get(http_request, "http-major", &major, "http-minor", &minor, NULL);
    g_object_set(response,
                 "http-major", major,
                 "http-minor", minor,
                 "status", 500,
                  NULL);

    IpcamHttpProcPrivate *priv = ipcam_http_proc_get_instance_private(http_proc);
    GList *item = g_list_first(priv->request_handler_list);
    for (; item; item = g_list_next(item))
    {
        g_object_set(item->data, "app", priv->iajax, NULL);
        if (ipcam_http_request_handler_dispatch(item->data, http_request, response, socket))
        {
            return response;
        }
    }
    ipcam_http_proc_error(http_proc, http_request, response);
    return response;
}
static void ipcam_http_proc_error(IpcamHttpProc *http_proc,
                                  IpcamHttpRequest *http_request,
                                  IpcamHttpResponse *http_response)
{
    guint major, minor;
    g_object_get(http_request, "http-major", &major, "http-minor", &minor, NULL);
    g_object_set(http_response,
                 "http-major", major,
                 "http-minor", minor,
                 "status", 400,
                 NULL);
}
