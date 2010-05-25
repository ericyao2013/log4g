/* Copyright 2010 Michael Steinert
 * This file is part of Log4g.
 *
 * Log4g is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * Log4g is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Log4g. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \brief Implements the API in log4g/appender/couchdb-appender.h
 * \author Mike Steinert
 * \date 5-20-2010
 *
 * TODO: investigate the bulk document API
 * http://wiki.apache.org/couchdb/HTTP_Bulk_Document_API
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "log4g/appender/couchdb-appender.h"
#include "log4g/layout/couchdb-layout.h"

#define GET_PRIVATE(instance) \
    (G_TYPE_INSTANCE_GET_PRIVATE(instance, LOG4G_TYPE_COUCHDB_APPENDER, \
            struct Log4gPrivate))

enum _properties_t {
    PROP_O = 0,
    PROP_URI,
    PROP_DATABASE_NAME,
    PROP_CREDENTIALS,
    PROP_MAX
};

struct Log4gPrivate {
    gchar *uri; /**< CouchDB server */
    gchar *name; /**< database name */
    CouchdbSession *session;
    CouchdbCredentials *credentials;
};

static void
activate_options(Log4gOptionHandler *base)
{
    struct Log4gPrivate *priv = GET_PRIVATE(base);
    if (priv->session) {
        g_object_unref(priv->session);
    }
    priv->session = couchdb_session_new(priv->uri);
    if (!priv->session) {
        log4g_log_error("couchdb_session_new() returned NULL");
        return;
    }
    log4g_log_debug("connected to CouchDB at %s",
            couchdb_session_get_uri(priv->session));
    if (priv->credentials) {
        couchdb_session_enable_authentication(priv->session,
                priv->credentials);
    }
    if (!priv->name) {
        log4g_log_warn("database not set for CouchDB connection at %s",
                couchdb_session_get_uri(priv->session));
        return;
    }
    GError *error = NULL;
    CouchdbDatabaseInfo *info =
        couchdb_session_get_database_info(priv->session, priv->name, &error);
    if (!info) {
        log4g_log_debug("%s: %s", priv->name, error->message);
        g_error_free(error);
        error = NULL;
        gboolean status =
            couchdb_session_create_database(priv->session, priv->name, &error);
        if (!status) {
            log4g_log_error("failed to create database %s: %s", priv->name,
                    error->message);
        }
        log4g_log_debug("%s: created new database", priv->name);
        return;
    }
    log4g_log_debug("%s: %d documents", couchdb_database_info_get_dbname(info),
            couchdb_database_info_get_documents_count(info));
    couchdb_database_info_unref(info);
}

static void
option_handler_init(Log4gOptionHandlerInterface *interface, gpointer data)
{
    interface->activate_options = activate_options;
}

static void
_close(Log4gAppender *base)
{
    struct Log4gPrivate *priv = GET_PRIVATE(base);
    if (!log4g_appender_skeleton_get_closed(base)) {
        log4g_appender_skeleton_set_closed(base, TRUE);
        if (priv->session) {
            g_object_unref(priv->session);
            priv->session = NULL;
        }
    }
}

static gboolean
requires_layout(Log4gAppender *self)
{
    return FALSE;
}

static void
appender_init(Log4gAppenderInterface *interface)
{
    interface->close = _close;
    interface->requires_layout = requires_layout;
}

G_DEFINE_DYNAMIC_TYPE_EXTENDED(Log4gCouchdbAppender, log4g_couchdb_appender,
        LOG4G_TYPE_APPENDER_SKELETON, 0,
        G_IMPLEMENT_INTERFACE(LOG4G_TYPE_OPTION_HANDLER, option_handler_init)
        G_IMPLEMENT_INTERFACE(LOG4G_TYPE_APPENDER, appender_init))

static void
log4g_couchdb_appender_init(Log4gCouchdbAppender *self)
{
    struct Log4gPrivate *priv = GET_PRIVATE(self);
    priv->uri = NULL;
    priv->name = g_strdup("log4g_messages");
    priv->session = NULL;
    priv->credentials = NULL;
    Log4gLayout *layout = log4g_couchdb_layout_new();
    if (layout) {
        log4g_appender_set_layout(LOG4G_APPENDER(self), layout);
    }
}

static void
dispose(GObject *base)
{
    struct Log4gPrivate *priv = GET_PRIVATE(base);
    if (priv->session) {
        g_object_unref(priv->session);
        priv->session = NULL;
    }
    if (priv->credentials) {
        g_object_unref(priv->credentials);
        priv->credentials = NULL;
    }
    G_OBJECT_CLASS(log4g_couchdb_appender_parent_class)->dispose(base);
}

static void
finalize(GObject *base)
{
    struct Log4gPrivate *priv = GET_PRIVATE(base);
    g_free(priv->uri);
    priv->uri = NULL;
    g_free(priv->name);
    priv->name = NULL;
    G_OBJECT_CLASS(log4g_couchdb_appender_parent_class)->finalize(base);
}

static void
append(Log4gAppender *base, Log4gLoggingEvent *event)
{
    Log4gLayout *layout = log4g_appender_get_layout(base);
    if (!layout) {
        return;
    }
    struct Log4gPrivate *priv = GET_PRIVATE(base);
    if (!priv->session) {
        log4g_log_error(Q_("attempted to append to a disconnected CouchDB "
                    "appender named [%s]"), log4g_appender_get_name(base));
        return;
    }
    if (!priv->name) {
        log4g_log_error(Q_("attempted to append to a CouchDB appender named "
                "[%s] without specifying a database"),
                log4g_appender_get_name(base));
        return;
    }
    if (LOG4G_IS_COUCHDB_LAYOUT(layout)) {
        CouchdbDocument *document =
            log4g_couchdb_layout_format_document(layout, event, priv->session);
        if (document) {
            GError *error = NULL;
            if (!couchdb_document_put(document, priv->name, &error)) {
                log4g_log_error(Q_("failed to store document in database "
                        "%s: %s"), priv->name, error->message);
                g_error_free(error);
            }
            g_object_unref(document);
        }
    }
}

static void
set_property(GObject *base, guint id, const GValue *value, GParamSpec *pspec)
{
    struct Log4gPrivate *priv = GET_PRIVATE(base);
    switch (id) {
    case PROP_URI:
        g_free(priv->uri);
        const gchar *uri = g_value_get_string(value);
        priv->uri = uri ? g_strdup(uri) : NULL;
        break;
    case PROP_DATABASE_NAME:
        g_free(priv->name);
        const gchar *name = g_value_get_string(value);
        priv->name = name ? g_strdup(name) : NULL;
        break;
    case PROP_CREDENTIALS:
        if (priv->credentials) {
            g_object_unref(priv->credentials);
        }
        CouchdbCredentials *credentials = g_value_get_object(value);
        if (credentials) {
            if (!COUCHDB_IS_CREDENTIALS(credentials)) {
                log4g_log_warn("object is not of type CouchdbCredentials");
                return;
            }
            g_object_ref(credentials);
            priv->credentials = credentials;
            if (priv->session) {
                couchdb_session_enable_authentication(priv->session,
                        priv->credentials);
            }
        } else {
            priv->credentials = NULL;
            if (priv->session) {
                couchdb_session_disable_authentication(priv->session);
            }
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(base, id, pspec);
        break;
    }
}

static void
log4g_couchdb_appender_class_init(Log4gCouchdbAppenderClass *klass)
{
    /* initialize GObject */
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = dispose;
    gobject_class->finalize = finalize;
    gobject_class->set_property = set_property;
    /* initialize private data */
    g_type_class_add_private(klass, sizeof(struct Log4gPrivate));
    /* initialize Log4gAppenderSkeleton */
    Log4gAppenderSkeletonClass *skeleton_class =
        LOG4G_APPENDER_SKELETON_CLASS(klass);
    skeleton_class->append = append;
    /* install properties */
    g_object_class_install_property(gobject_class, PROP_URI,
            g_param_spec_string("uri", Q_("URI"),
                    Q_("URI of a CouchDB server"), NULL, G_PARAM_WRITABLE));
    g_object_class_install_property(gobject_class, PROP_DATABASE_NAME,
            g_param_spec_string("database-name", Q_("Database name"),
                    Q_("Name of a CouchDB database"), "log4g_messages",
                    G_PARAM_WRITABLE));
    g_object_class_install_property(gobject_class, PROP_CREDENTIALS,
            g_param_spec_object("credentials", Q_("Database credentials"),
                    Q_("Credentials for a CouchDB database"),
                    COUCHDB_TYPE_CREDENTIALS, G_PARAM_WRITABLE));
}

static void
log4g_couchdb_appender_class_finalize(Log4gCouchdbAppenderClass *klass)
{
    /* do nothing */
}

void
log4g_couchdb_appender_register(GTypeModule *base)
{
    log4g_couchdb_appender_register_type(base);
}

Log4gAppender *
log4g_couchdb_appender_new(const gchar *uri, const gchar *name)
{
    Log4gAppender *self = g_object_new(LOG4G_TYPE_COUCHDB_APPENDER, NULL);
    if (!self) {
        return NULL;
    }
    if (uri) {
        g_object_set(self, "uri", uri, NULL);
    }
    if (name) {
        g_object_set(self, "database-name", name, NULL);
    }
    log4g_option_handler_activate_options(LOG4G_OPTION_HANDLER(self));
    return self;
}

void
log4g_couchdb_appender_set_uri(Log4gAppender *base, const gchar *uri)
{
    g_return_if_fail(LOG4G_IS_COUCHDB_APPENDER(base));
    g_object_set(base, "uri", uri, NULL);
}

const gchar *
log4g_couchdb_appender_get_uri(Log4gAppender *base)
{
    g_return_val_if_fail(LOG4G_IS_COUCHDB_APPENDER(base), NULL);
    return GET_PRIVATE(base)->uri;
}

void
log4g_couchdb_appender_set_database_name(Log4gAppender *base,
        const gchar *name)
{
    g_return_if_fail(LOG4G_IS_COUCHDB_APPENDER(base));
    g_object_set(base, "database-name", name, NULL);
}

const gchar *
log4g_couchdb_appender_get_database_name(Log4gAppender *base)
{
    g_return_val_if_fail(LOG4G_IS_COUCHDB_APPENDER(base), NULL);
    return GET_PRIVATE(base)->name;
}

void
log4g_couchdb_appender_set_credentials(Log4gAppender *base,
        CouchdbCredentials *credentials)
{
    g_return_if_fail(LOG4G_IS_COUCHDB_APPENDER(base));
    g_object_set(base, "credentials", credentials, NULL);
}

CouchdbCredentials *
log4g_couchdb_appender_get_credentials(Log4gAppender *base)
{
    g_return_val_if_fail(LOG4G_IS_COUCHDB_APPENDER(base), NULL);
    return GET_PRIVATE(base)->credentials;
}