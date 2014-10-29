#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "iajax.h"
#include "event-handler.h"
#include "ajax/ajax.h"
#include "common.h"

#define PROPERTY_PATH_MAX_DEPTH  5
#define CONNECTION_TIMEOUT       10 /* seconds */

typedef struct _IpcamIAjaxPrivate
{
    IpcamAjax *ajax;
    //IpcamLoginInfoManager *login_manager;
    GHashTable *cached_users_hash;
    GHashTable *cached_property_hash;
    GHashTable *connection_hash;
    gchar *property_path[PROPERTY_PATH_MAX_DEPTH];
    guint property_path_size;
    GMutex users_mutex;
    GMutex property_mutex;
    GMutex connection_mutex;
} IpcamIAjaxPrivate;

typedef struct _IpcamIAjaxUsersHashValue
{
    gchar *name;
    gchar *pwd;
    gchar *role;
} IpcamIAjaxUsersHashValue;

typedef struct _IpcamIAjaxConnectionHashValue
{
    GSocket *socket;
    IpcamHttpResponse *response;
    guint timeout;
} IpcamIAjaxConnectionHashValue;

#define USER IpcamIAjaxUsersHashValue
#define CONNECTION IpcamIAjaxConnectionHashValue

G_DEFINE_TYPE_WITH_PRIVATE(IpcamIAjax, ipcam_iajax, IPCAM_BASE_APP_TYPE);

static void ipcam_iajax_before_start(IpcamBaseService *base_service);
static void ipcam_iajax_in_loop(IpcamBaseService *base_service);

static void get_message_handler(GObject *obj, IpcamMessage* msg, gboolean timeout);
static void set_message_handler(GObject *obj, IpcamMessage* msg, gboolean timeout);

static void ipcam_iajax_query_misc(IpcamIAjax *iajax);
static void ipcam_iajax_query_base_info(IpcamIAjax *iajax);
static void ipcam_iajax_query_users(IpcamIAjax *iajax);
static void ipcam_iajax_query_datetime(IpcamIAjax *iajax);
static void ipcam_iajax_query_video(IpcamIAjax *iajax);
static void ipcam_iajax_query_image(IpcamIAjax *iajax);
static void ipcam_iajax_query_privacy_block(IpcamIAjax *iajax);
static void ipcam_iajax_query_day_night_mode(IpcamIAjax *iajax);
static void ipcam_iajax_query_osd(IpcamIAjax *iajax);
static void ipcam_iajax_query_szyc(IpcamIAjax *iajax);
static void ipcam_iajax_query_network(IpcamIAjax *iajax);
static void ipcam_iajax_query_event_input(IpcamIAjax *iajax);
static void ipcam_iajax_query_event_output(IpcamIAjax *iajax);
static void ipcam_iajax_query_event_motion(IpcamIAjax *iajax);
static void ipcam_iajax_query_event_cover(IpcamIAjax *iajax);
static void ipcam_iajax_query_event_proc(IpcamIAjax *iajax);

static void ipcam_iajax_connection_clear(GObject *obj);

static void property_value_destroy_func(gpointer value)
{
	g_variant_unref(value);
}

static void users_value_destroy_func(gpointer value)
{
    USER *user = (USER *)value;
    g_free(user->name);
    g_free(user->pwd);
    g_free(user->role);
    g_free(user);
}

static void connection_value_destroy_func(gpointer value)
{
    CONNECTION *conn = (CONNECTION *)value;
    if (conn->socket && conn->response)
    {
        g_object_set(conn->response, "status", 408, NULL);
        ipcam_http_response_write_string(conn->response, conn->socket);
        g_clear_object(&conn->response);
        g_socket_close(conn->socket, NULL);
        g_clear_object(&conn->socket);
    }
    g_free(conn);
}

static void ipcam_iajax_finalize(GObject *object)
{
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(IPCAM_IAJAX(object));
    g_mutex_lock(&priv->connection_mutex);
    g_hash_table_remove_all(priv->connection_hash);
    g_object_unref(priv->connection_hash);
    g_mutex_unlock(&priv->connection_mutex);
    g_mutex_clear(&priv->connection_mutex);
    
    g_mutex_lock(&priv->users_mutex);
    g_hash_table_remove_all(priv->cached_users_hash);
    g_object_unref(priv->cached_users_hash);
    g_mutex_unlock(&priv->users_mutex);
    g_mutex_clear(&priv->users_mutex);

    g_mutex_lock(&priv->property_mutex);
    g_hash_table_remove_all(priv->cached_property_hash);
    g_object_unref(priv->cached_property_hash);
    g_mutex_unlock(&priv->property_mutex);
    g_mutex_clear(&priv->property_mutex);
    
    g_clear_object(&priv->ajax);
    G_OBJECT_CLASS(ipcam_iajax_parent_class)->finalize(object);
}

static void ipcam_iajax_init(IpcamIAjax *self)
{
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(self);
    priv->property_path_size = 0;
    g_mutex_init(&priv->users_mutex);
    g_mutex_init(&priv->property_mutex);
    g_mutex_init(&priv->connection_mutex);
    priv->cached_property_hash = g_hash_table_new_full(g_str_hash,
                                                       g_str_equal,
	                                                   g_free,
	                                                   property_value_destroy_func);
    priv->cached_users_hash = g_hash_table_new_full(g_str_hash,
                                                    g_str_equal,
                                                    g_free,
                                                    users_value_destroy_func);
    priv->connection_hash = g_hash_table_new_full(g_str_hash,
                                                  g_str_equal,
                                                  g_free,
                                                  connection_value_destroy_func);
}

static void ipcam_iajax_class_init(IpcamIAjaxClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = &ipcam_iajax_finalize;
    
    IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
    base_service_class->before = ipcam_iajax_before_start;
    base_service_class->in_loop = ipcam_iajax_in_loop;
}

static void ipcam_iajax_before_start(IpcamBaseService *base_service)
{
    IpcamIAjax *iajax = IPCAM_IAJAX(base_service);
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    const gchar *ajax_addr = ipcam_base_app_get_config(IPCAM_BASE_APP(iajax), "ajax:address");
    const gchar *port = ipcam_base_app_get_config(IPCAM_BASE_APP(iajax), "ajax:port");

    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_misc", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_base_info", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_users", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "add_users", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "del_users", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_datetime", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_video", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_image", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_privacy_block", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_day_night_mode", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_osd", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_szyc", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_network", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_event_input", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_event_output", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_event_motion", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_event_cover", IPCAM_TYPE_IAJAX_EVENT_HANDLER);
    ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(iajax), "set_event_proc", IPCAM_TYPE_IAJAX_EVENT_HANDLER);

    ipcam_iajax_query_misc(iajax);
    ipcam_iajax_query_base_info(iajax);
    ipcam_iajax_query_users(iajax);
    ipcam_iajax_query_datetime(iajax);
    ipcam_iajax_query_video(iajax);
    ipcam_iajax_query_image(iajax);
    ipcam_iajax_query_privacy_block(iajax);
    ipcam_iajax_query_day_night_mode(iajax);
    ipcam_iajax_query_osd(iajax);
    ipcam_iajax_query_szyc(iajax);
    ipcam_iajax_query_network(iajax);
    ipcam_iajax_query_event_input(iajax);
    ipcam_iajax_query_event_output(iajax);
    ipcam_iajax_query_event_motion(iajax);
    ipcam_iajax_query_event_cover(iajax);
    ipcam_iajax_query_event_proc(iajax);

    if (ajax_addr != NULL && port != NULL)
    {
        priv->ajax = g_object_new(IPCAM_AJAX_TYPE,
                                  "app", iajax,
                                  "address", ajax_addr,
                                  "port", strtol(port, NULL, 10),
                                  NULL);
    }

    ipcam_base_app_add_timer(IPCAM_BASE_APP(iajax), "connection_clear", "1",
                             ipcam_iajax_connection_clear);
}

static void ipcam_iajax_in_loop(IpcamBaseService *base_service)
{
}

static const GVariant *ipcam_iajax_get_property(IpcamIAjax *iajax, const gchar *key)
{
	IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
	GVariant *value;

	g_mutex_lock(&priv->property_mutex);
	value = g_hash_table_lookup(priv->cached_property_hash, key);
    g_variant_ref(value);
	g_mutex_unlock(&priv->property_mutex);

	return value;
}

static void ipcam_iajax_set_property(IpcamIAjax *iajax, gchar *key, GVariant *value)
{
    g_return_if_fail(value);
	IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);

	g_mutex_lock(&priv->property_mutex);
	g_hash_table_insert(priv->cached_property_hash, key, value);
	g_mutex_unlock(&priv->property_mutex);
}

static void ipcam_iajax_proc_property_value(IpcamIAjax *iajax, const gchar *name, JsonNode *node)
{
    GVariant *value = NULL;
    gchar *path = NULL;
    gchar *key = NULL;
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);

    path = g_strjoinv(":", priv->property_path);
    asprintf(&key, "%s:%s", path, name);
    g_free(path);
    switch (json_node_get_value_type(node))
    {
    case G_TYPE_STRING:
        value = g_variant_new_string(json_node_get_string(node));
        break;
    case G_TYPE_BOOLEAN:
        value = g_variant_new_boolean(json_node_get_boolean(node));
        break;
    case G_TYPE_INT:
        value = g_variant_new_int32(json_node_get_int(node));
        break;
    case G_TYPE_UINT:
        value = g_variant_new_uint32(json_node_get_int(node));
        break;
    case G_TYPE_INT64:
        value = g_variant_new_int64(json_node_get_int(node));
        break;
    default:
        g_print("json node value type is: %s\n", g_type_name(json_node_get_value_type(node)));
        g_warn_if_reached();
        break;
    }

    if (key && value)
    {
        ipcam_iajax_set_property(iajax, key, value);
    }
}

static void property_object_proc(JsonObject *object,
                                 const gchar *member_name,
                                 JsonNode *member_node,
                                 gpointer user_data)
{
    IpcamIAjax *iajax = IPCAM_IAJAX(user_data);
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    
    switch (json_node_get_node_type(member_node))
    {
    case JSON_NODE_OBJECT:
        if (priv->property_path_size < PROPERTY_PATH_MAX_DEPTH)
        {   
            priv->property_path[priv->property_path_size] = member_name;
            priv->property_path_size++;
            json_object_foreach_member(json_node_get_object(member_node), property_object_proc, iajax);
            priv->property_path_size--;
            priv->property_path[priv->property_path_size] = NULL;
        }
        break;
    case JSON_NODE_VALUE:
        ipcam_iajax_proc_property_value(iajax, member_name, member_node);
        break;
    default:
        g_warn_if_reached();
        break;
    }
}

static void property_array_proc(JsonArray *array,
                                guint index_,
                                JsonNode *element_node,
                                gpointer user_data)
{
    IpcamIAjax *iajax = IPCAM_IAJAX(user_data);
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);

    JsonObject *user_obj = json_node_get_object(element_node);
    if (user_obj)
    {   
        USER *user = g_new(USER, 1);
        user->name = g_strdup(json_object_get_string_member(user_obj, "username"));
        if (g_str_has_prefix(priv->property_path[0], "del_"))
        {
            g_mutex_lock(&priv->users_mutex);
            g_hash_table_remove(priv->cached_users_hash, user->name);
            g_mutex_unlock(&priv->users_mutex);
            g_free(user->name);
            g_free(user);
        }
        else
        {
            user->pwd = g_strdup(json_object_get_string_member(user_obj, "password"));
            user->role = g_strdup(json_object_get_string_member(user_obj, "role"));
            g_mutex_lock(&priv->users_mutex);
            g_hash_table_insert(priv->cached_users_hash, g_strdup(user->name), user);
            g_mutex_unlock(&priv->users_mutex);
        }
    }
}

static void ipcam_iajax_update_cached_property(IpcamIAjax *iajax, const gchar *name, JsonNode *body)
{
    g_return_if_fail(JSON_NODE_HOLDS_OBJECT(body));
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    
    /* only get set add del users return a json array */
    if (g_strstr_len(name, -1, "users"))
    {
        JsonArray *items_ary = json_object_get_array_member(json_node_get_object(body), "items");
        priv->property_path[priv->property_path_size] = name;
        priv->property_path_size++;
        json_array_foreach_element(items_ary, property_array_proc, iajax);
        priv->property_path_size--;
        priv->property_path[priv->property_path_size] = NULL;
    }
    else
    {
        JsonObject *items_obj = json_object_get_object_member(json_node_get_object(body), "items");
    
        priv->property_path[priv->property_path_size] = name;
        priv->property_path_size++;
        json_object_foreach_member(items_obj, property_object_proc, iajax);
        priv->property_path_size--;
        priv->property_path[priv->property_path_size] = NULL;
    }
}

static void get_message_handler(GObject *obj, IpcamMessage *msg, gboolean timeout)
{
	IpcamIAjax *iajax = IPCAM_IAJAX(obj);
	g_return_if_fail(IPCAM_IS_IAJAX(iajax));

	if (!timeout && msg)
    {
        gchar *action = NULL;
		JsonNode *body = NULL;
		g_object_get(msg, "action", &action, "body", &body, NULL);
		if (action && body)
        {
            ipcam_iajax_property_handler(iajax, action, body);
        }

        g_free(action);
	}
}

static void ipcam_iajax_send_response(IpcamIAjax *iajax, const gchar *id, JsonNode *body)
{
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    g_mutex_lock(&priv->connection_mutex);
    CONNECTION *conn = g_hash_table_lookup(priv->connection_hash, id);
    gchar *result;
    JsonGenerator *generator;
    generator = json_generator_new();
    json_generator_set_root(generator, body);
    json_generator_set_pretty(generator, TRUE);
    result = json_generator_to_data(generator, NULL);
    if (conn)
    {
        g_object_set(conn->response,
                     "status", 200,
                     "body", result,
                     NULL);
        ipcam_http_response_write_string(conn->response, conn->socket);

        g_clear_object(&conn->response);
        conn->response = NULL;
        g_socket_close(conn->socket, NULL);
        g_clear_object(&conn->socket);
        conn->socket = NULL;
        
        g_hash_table_remove(priv->connection_hash, id);
    }
    g_mutex_unlock(&priv->connection_mutex);
    g_free(result);
    g_object_unref(G_OBJECT(generator));
}

static void set_message_handler(GObject *obj, IpcamMessage* msg, gboolean timeout)
{
    IpcamIAjax *iajax = IPCAM_IAJAX(obj);
	g_return_if_fail(IPCAM_IS_IAJAX(iajax));

	if (!timeout && msg)
    {
        gchar *msg_id = NULL;
		JsonNode *body = NULL;
		g_object_get(msg, "id", &msg_id, "body", &body, NULL);
		if (msg_id && body)
        {
            ipcam_iajax_send_response(iajax, msg_id, body);
        }

        g_free(msg_id);
	}
}

static void ipcam_iajax_send_query_msg(IpcamIAjax *iajax, const gchar *action, JsonBuilder *builder)
{
	const gchar *token = ipcam_base_app_get_config(IPCAM_BASE_APP(iajax), "token");
	IpcamRequestMessage *req_msg;
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", action,
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(iajax), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, get_message_handler, 5);
    g_object_unref(req_msg);
}

static void ipcam_iajax_query_misc(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    /* Request the misc */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "language");
	json_builder_add_string_value(builder, "rtsp_auth");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_misc", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_base_info(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    /* Request the Base Information */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "device_name");
	json_builder_add_string_value(builder, "comment");
	json_builder_add_string_value(builder, "location");
	json_builder_add_string_value(builder, "hardware");
	json_builder_add_string_value(builder, "firmware");
	json_builder_add_string_value(builder, "manufacturer");
	json_builder_add_string_value(builder, "model");
	json_builder_add_string_value(builder, "serial");
    json_builder_add_string_value(builder, "hwaddr");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_base_info", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_users(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "password");
	json_builder_add_string_value(builder, "role");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_users", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_datetime(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    /* Request the datetime */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "timezone");
	json_builder_add_string_value(builder, "use_ntp");
	json_builder_add_string_value(builder, "ntp_server");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_datetime", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_video(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "profile");
	json_builder_add_string_value(builder, "flip");
	json_builder_add_string_value(builder, "mirror");
	json_builder_add_string_value(builder, "main_profile");
    json_builder_add_string_value(builder, "sub_profile");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_video", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_image(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "watermark");
	json_builder_add_string_value(builder, "3ddnr");
	json_builder_add_string_value(builder, "brightness");
	json_builder_add_string_value(builder, "chrominance");
    json_builder_add_string_value(builder, "contrast");
    json_builder_add_string_value(builder, "saturation");
    json_builder_add_string_value(builder, "scenario");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_image", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_privacy_block(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "region1");
	json_builder_add_string_value(builder, "region2");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_privacy_block", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_day_night_mode(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "force_night_mode");
	json_builder_add_string_value(builder, "night_mode_threshold");
    json_builder_add_string_value(builder, "ir_intensity");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_day_night_mode", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_osd(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "master:device_name");
	json_builder_add_string_value(builder, "master:comment");
    json_builder_add_string_value(builder, "master:datetime");
    json_builder_add_string_value(builder, "master:frame_rate");
	json_builder_add_string_value(builder, "master:bit_rate");
    json_builder_add_string_value(builder, "slave:device_name");
    json_builder_add_string_value(builder, "slave:comment");
	json_builder_add_string_value(builder, "slave:datetime");
    json_builder_add_string_value(builder, "slave:frame_rate");
    json_builder_add_string_value(builder, "slave:bit_rate");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_osd", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_szyc(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "train_num");
	json_builder_add_string_value(builder, "carriage_num");
    json_builder_add_string_value(builder, "position_num");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_szyc", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_network(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "method");
	json_builder_add_string_value(builder, "address");
	json_builder_add_string_value(builder, "pppoe");
	json_builder_add_string_value(builder, "port");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_network", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_event_input(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "input1");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_event_input", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_event_output(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "output1");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_event_output", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_event_motion(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
    json_builder_add_string_value(builder, "region1");
    json_builder_add_string_value(builder, "region2");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_event_motion", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_event_cover(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
    json_builder_add_string_value(builder, "region1");
    json_builder_add_string_value(builder, "region2");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_event_cover", builder);
	g_object_unref(builder);
}

static void ipcam_iajax_query_event_proc(IpcamIAjax *iajax)
{
    JsonBuilder *builder;
    builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
    json_builder_add_string_value(builder, "input1");
    json_builder_add_string_value(builder, "motion");
    json_builder_add_string_value(builder, "cover");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    ipcam_iajax_send_query_msg(iajax, "get_event_proc", builder);
	g_object_unref(builder);
}

static gboolean connection_clear(gpointer key, gpointer value, gpointer data)
{
    CONNECTION *conn = (CONNECTION *)value;
    conn->timeout++;
    if (conn->timeout > CONNECTION_TIMEOUT)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void ipcam_iajax_connection_clear(GObject *obj)
{
    g_return_if_fail(IPCAM_IS_IAJAX(obj));
    IpcamIAjax *iajax = IPCAM_IAJAX(obj);
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);

    g_mutex_lock(&priv->connection_mutex);
    g_hash_table_foreach_remove(priv->connection_hash, connection_clear, iajax);
    g_mutex_unlock(&priv->connection_mutex);
}

void ipcam_iajax_property_handler(IpcamIAjax *iajax, const gchar *name, JsonNode *body)
{
    g_return_if_fail(IPCAM_IS_IAJAX(iajax));

    if (g_str_has_prefix(name, "set_") ||
        g_str_has_prefix(name, "get_") ||
        g_str_has_prefix(name, "add_"))
    {
        ipcam_iajax_update_cached_property(iajax, name + 4, body);
    }
    else
    {
        /* if name is del_xxx then call function with full name */
        ipcam_iajax_update_cached_property(iajax, name, body);
    }
}

GVariant *ipcam_iajax_get_configuration(IpcamIAjax *iajax, const gchar *name)
{
    g_return_val_if_fail(IPCAM_IS_IAJAX(iajax), NULL);
    return ipcam_iajax_get_property(iajax, name);
}

void ipcam_iajax_set_configuration(IpcamIAjax *iajax, IpcamRequestMessage *msg,
                                   IpcamHttpResponse *response, GSocket *socket)
{
    g_return_if_fail(IPCAM_IS_IAJAX(iajax));
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    const gchar *token = ipcam_base_app_get_config(IPCAM_BASE_APP(iajax), "token");
    gchar *id = NULL;

    g_object_get(msg, "id", &id, NULL);
    if (id)
    {
        ipcam_base_app_send_message(IPCAM_BASE_APP(iajax), IPCAM_MESSAGE(msg),
                                    "iconfig", token, set_message_handler, 5);
        
        CONNECTION *conn = g_new(CONNECTION, 1);
        conn->response = response;
        conn->socket = socket;
        conn->timeout = 0;
        g_mutex_lock(&priv->connection_mutex);
        g_hash_table_insert(priv->connection_hash, id, conn);
        g_mutex_unlock(&priv->connection_mutex);
    }
}

GList *ipcam_iajax_get_users(IpcamIAjax *iajax)
{
    g_return_val_if_fail(IPCAM_IS_IAJAX(iajax), NULL);
    GList *users = NULL;
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    g_mutex_lock(&priv->users_mutex);
    users = g_list_copy_deep(g_hash_table_get_keys(priv->cached_users_hash),
                             (GCopyFunc)g_strdup, NULL);
    g_mutex_unlock(&priv->users_mutex);
    return users;
}

gchar *ipcam_iajax_get_user_pwd(IpcamIAjax *iajax, const gchar *name)
{
    g_return_val_if_fail(IPCAM_IS_IAJAX(iajax), NULL);
    gchar *pwd = NULL;
    USER *user;
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    g_mutex_lock(&priv->users_mutex);
    user = g_hash_table_lookup(priv->cached_users_hash, name);
    if (user)
    {
        pwd = g_strdup(user->pwd);
    }
    g_mutex_unlock(&priv->users_mutex);
    return pwd;
}

gchar *ipcam_iajax_get_user_role(IpcamIAjax *iajax, const gchar *name)
{
    g_return_val_if_fail(IPCAM_IS_IAJAX(iajax), NULL);
    gchar *role = NULL;
    USER *user;
    IpcamIAjaxPrivate *priv = ipcam_iajax_get_instance_private(iajax);
    g_mutex_lock(&priv->users_mutex);
    user = g_hash_table_lookup(priv->cached_users_hash, name);
    if (user)
    {
        role = g_strdup(user->role);
    }
    g_mutex_unlock(&priv->users_mutex);
    return role;
}
