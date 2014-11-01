#include "logout_handler.h"
#include "ajax/http_request.h"
#include "ajax/http_response.h"
#include "ajax/http_query_string_parser.h"
#include "iajax.h"
#include "login/sha1.h"

G_DEFINE_TYPE(IpcamHttpLogoutHandler, ipcam_http_logout_handler, IPCAM_HTTP_REQUEST_HANDLER_TYPE)

START_HANDLER(get_logout, HTTP_GET, "/api/1.0/logout.json", http_request, http_response, socket)
{
    IpcamIAjax *iajax;
    gchar *token = NULL;
    
    g_object_get(get_logout, "app", &iajax, NULL);

    gchar *cookie = ipcam_http_request_get_header_value(http_request, "Set-Cookie");
    if (cookie)
    {
        gchar *token_start = g_strstr_len(cookie, -1, "token=");
        if (token_start)
        {
            token = g_malloc0(SHA1_DIGEST_SIZE * 2 + 1);
            guint i = 0;
            if (token)
            {
                do 
                {
                    token[i] = token_start[6 + i];
                    i++;
                } while (!(token_start[6 + i] == '\0' || token_start[6 + i] == ';'));
                    
                ipcam_iajax_logout(iajax, token);
                g_free(token);
            }
        }
        g_free(cookie);
    }

    g_object_set(http_response, "status", 200, NULL);
    
    ret = TRUE;
}
END_HANDLER

static void ipcam_http_logout_handler_init(IpcamHttpLogoutHandler *self)
{
    ipcam_http_request_handler_register(IPCAM_HTTP_REQUEST_HANDLER(self), get_logout);
}

static void ipcam_http_logout_handler_class_init(IpcamHttpLogoutHandlerClass *klass)
{
}
