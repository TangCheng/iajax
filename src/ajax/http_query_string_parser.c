#include "http_query_string_parser.h"

typedef struct _IpcamHttpQueryStringParserPrivate
{
    GHashTable *query_hash;
} IpcamHttpQueryStringParserPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IpcamHttpQueryStringParser, ipcam_http_query_string_parser, G_TYPE_OBJECT)

static void destroy_key(gpointer data)
{
    g_free(data);
}
void destroy_value(gpointer data)
{
    GList *item = g_list_first(data);
    for (; item; item = g_list_next(item))
    {
        g_free(item->data);
    }
    g_list_free(data);
}
static void ipcam_http_query_string_parser_finalize(GObject *object)
{
    IpcamHttpQueryStringParserPrivate *priv =
        ipcam_http_query_string_parser_get_instance_private(IPCAM_HTTP_QUERY_STRING_PARSER(object));
    g_hash_table_destroy(priv->query_hash);
    G_OBJECT_CLASS(ipcam_http_query_string_parser_parent_class)->finalize(object);
}
static void ipcam_http_query_string_parser_init(IpcamHttpQueryStringParser *self)
{
	IpcamHttpQueryStringParserPrivate *priv = ipcam_http_query_string_parser_get_instance_private(self);
    priv->query_hash = g_hash_table_new_full(g_str_hash, g_str_equal, destroy_key, destroy_value);
}
static void ipcam_http_query_string_parser_class_init(IpcamHttpQueryStringParserClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = &ipcam_http_query_string_parser_finalize;
}
#define ISHEX(x)   ((((x)>='0'&&(x)<='9') || ((x)>='A'&&(x)<='F') || ((x)>='a'&&(x)<='f')) ? TRUE : FALSE)
#define HEX2DEC(x) (((x)>='0'&&(x)<='9') ? (x)-48 : ((x)>='A'&&(x)<='F') ? (x)-55 : ((x)>='a'&&(x)<='f') ? (x)-87 : 0)
#define ISQSCHR(x) ((((x)=='=')||((x)=='#')||((x)=='&')||((x)=='\0')) ? FALSE : TRUE)
static gchar *ipcam_http_query_string_parser_prepare(IpcamHttpQueryStringParser *parser, gchar *query_string)
{
    gchar *ret = g_strdup(query_string);
    int i = 0, j = 0;

    while (query_string[j])
    {
        if (ret[j] == '+')
        {
            ret[i] = ' ';
        }
        else if (ret[j] == '%') // easier/safer than scanf
        {
            if (!ISHEX(ret[j + 1]) || !ISHEX(ret[j + 2]))
            {
                ret[i] = '\0';
                break;
            }
            ret[i] = (HEX2DEC(ret[j + 1]) * 16) + HEX2DEC(ret[j + 2]);
            j += 2;
        }
        else
        {
            ret[i] = ret[j];
        }
        i++;
        j++;
    }
    ret[i] = '\0';
    return ret;
}
GHashTable *ipcam_http_query_string_parser_get(IpcamHttpQueryStringParser *parser,
                                               gchar *query_string)
{
    g_return_val_if_fail(IPCAM_IS_HTTP_QUERY_STRING_PARSER(parser), NULL);
    int pos = 0;
    gchar tmp[32] = {0};
    GList *list = NULL;
    gchar *key = NULL;
    gchar *value = NULL;
    gchar *query = ipcam_http_query_string_parser_prepare(parser, query_string);
    IpcamHttpQueryStringParserPrivate *priv = ipcam_http_query_string_parser_get_instance_private(parser);
    char *p = query;
    g_hash_table_remove_all(priv->query_hash);
    
    while (*p)
    {
        if (*p == '&' || *(p + 1) == '\0')
        {
            if (*(p + 1) == '\0')
            {
                tmp[pos] = *p;
                pos++;
            }
            tmp[pos] = '\0';
            value = g_strdup(tmp);
            pos = 0;

            list = g_hash_table_lookup(priv->query_hash, key);
            if (!list)
            {
                list = g_list_append(list, value);
                g_hash_table_insert(priv->query_hash, key, list);
            }
            else
            {
                g_free(key);
                list = g_list_append(list, value);
            }
        }
        else if (*p == '=')
        {
            tmp[pos] = '\0';
            key = g_strdup(tmp);
            pos = 0;
        }
        else
        {
            tmp[pos] = *p;
            pos++;
        }
        p++;
    }
    g_free(query);
    return priv->query_hash;
}
