/* Copyright 2010, 2011 Michael Steinert
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

#ifndef LOG4G_ROOT_LOGGER_H
#define LOG4G_ROOT_LOGGER_H

#include <log4g/logger.h>

G_BEGIN_DECLS

#define LOG4G_TYPE_ROOT_LOGGER \
	(log4g_root_logger_get_type())

#define LOG4G_ROOT_LOGGER(instance) \
	(G_TYPE_CHECK_INSTANCE_CAST((instance), LOG4G_TYPE_ROOT_LOGGER, \
		Log4gRootLogger))

#define LOG4G_IS_ROOT_LOGGER(instance) \
	(G_TYPE_CHECK_INSTANCE_TYPE((instance), LOG4G_TYPE_ROOT_LOGGER))

#define LOG4G_ROOT_LOGGER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), LOG4G_TYPE_ROOT_LOGGER, \
		Log4gRootLoggerClass))

#define LOG4G_IS_ROOT_LOGGER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass), LOG4G_TYPE_ROOT_LOGGER))

#define LOG4G_ROOT_LOGGER_GET_CLASS(instance) \
	(G_TYPE_INSTANCE_GET_CLASS((instance), LOG4G_TYPE_ROOT_LOGGER, \
		Log4gRootLoggerClass))

typedef struct Log4gRootLogger_ Log4gRootLogger;

typedef struct Log4gRootLoggerClass_ Log4gRootLoggerClass;

/**
 * Log4gRootLogger:
 *
 * The <structname>Log4gRootLogger</structname> structure does not have any
 * public members.
 */
struct Log4gRootLogger_ {
	/*< private >*/
	Log4gLogger parent_instance;
	gpointer priv;
};

/**
 * Log4gRootLoggerClass:
 *
 * The <structname>Log4gRootLogger</structname> structure does not have any
 * public members.
 */
struct Log4gRootLoggerClass_ {
	/*< private >*/
	Log4gLoggerClass parent_class;
};

GType
log4g_root_logger_get_type(void) G_GNUC_CONST;

Log4gLogger *
log4g_root_logger_new(Log4gLevel *level);

G_END_DECLS

#endif /* LOG4G_ROOT_LOGGER_H */
