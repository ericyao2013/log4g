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
 * \file
 * \brief Log output interface.
 * \author Mike Steinert
 * \date 1-29-2010
 *
 * Implement this interface to define your own strategy for outputting log
 * statement.
 */

#ifndef LOG4G_APPENDER_H
#define LOG4G_APPENDER_H

#include <log4g/filter.h>
#include <log4g/interface/option-handler.h>
#include <log4g/layout.h>

G_BEGIN_DECLS

#define LOG4G_TYPE_APPENDER \
    (log4g_appender_get_type())

#define LOG4G_APPENDER(instance) \
    (G_TYPE_CHECK_INSTANCE_CAST((instance), LOG4G_TYPE_APPENDER, \
            Log4gAppender))

#define LOG4G_IS_APPENDER(instance) \
    (G_TYPE_CHECK_INSTANCE_TYPE((instance), LOG4G_TYPE_APPENDER))

#define LOG4G_APPENDER_GET_INTERFACE(instance) \
    (G_TYPE_INSTANCE_GET_INTERFACE((instance), LOG4G_TYPE_APPENDER, \
            Log4gAppenderInterface));

/** \brief Log4gAppender object type definition */
typedef struct _Log4gAppender Log4gAppender;

/** \brief Log4gAppender interface type definition */
typedef struct _Log4gAppenderInterface Log4gAppenderInterface;

/** \brief Log4gAppenderInterface definition */
struct _Log4gAppenderInterface {
    GTypeInterface parent_interface;

    /**
     * \brief Add a filter to the end of the filter chain.
     *
     * \param self [in] An appender object.
     * \param filter [in] A filter to add to \e self.
     *
     * \see log4g/filter.h
     */
    void
    (*add_filter)(Log4gAppender *self, Log4gFilter *filter);

    /**
     * Filters are organized in a linked list so all filters are available
     * through the result of this function.
     *
     * \param self [in] An appender object.
     *
     * \return The first filter in the filter chain.
     *
     * \see log4g/filter.h
     */
    Log4gFilter *
    (*get_filter)(Log4gAppender *self);

    /**
     * \brief Release all resources.
     *
     * Release any resources allocated within the appender such as file
     * handles, network connections, etc. A closed appender is no longer
     * usable by the Log4g system.
     *
     * \param self [in] An appender object.
     */
    void
    (*close)(Log4gAppender *self);

    /**
     * \brief Log in an appender-specific way.
     *
     * Loggers will call this function in order to log events.
     *
     * \param self [in] An appender object.
     */
    void
    (*do_append)(Log4gAppender *self, Log4gLoggingEvent *event);

    /**
     * \brief Retrieve the name an appender.
     *
     * This name uniquely identifies the appender.
     *
     * \param self [in] An appender object.
     *
     * \return The name of \e self (may be \e NULL).
     */
    const gchar *
    (*get_name)(Log4gAppender *self);

    /**
     * \brief set the error handler for an appender.
     *
     * \param self [in] An appender object.
     * \param handler [in] An error handler.
     *
     * \see log4g/interface/error-handler.h
     */
    void
    (*set_error_handler)(Log4gAppender *self, gpointer handler);

    /**
     * \brief Retrieve the error handler for an appender.
     *
     * \param self [in] An appender object.
     *
     * \return The error handler set for \e self.
     *
     * \see log4g/interface/error-handler.h
     */
    gpointer
    (*get_error_handler)(Log4gAppender *self);

    /**
     * Set the layout for an appender.
     *
     * \param self [in] An appender object.
     * \param layout [in] The new layout for \e self.
     *
     * \see log4g/layout.h
     */
    void
    (*set_layout)(Log4gAppender *self, Log4gLayout *layout);

    /**
     * \brief Retrieve the layout for an appender.
     *
     * \param self [in] An appender object.
     *
     * \return The layout for \e self.
     *
     * \see log4g/layout.h
     */
    Log4gLayout *
    (*get_layout)(Log4gAppender *self);

    /**
     * \brief Set the name of an appender.
     *
     * The name may be used by other components to uniquely identify the
     * appender.
     *
     * \param self [in] An appender object.
     * \param name [in] The new name for \e self.
     */
    void
    (*set_name)(Log4gAppender *self, const gchar *name);

    /**
     * \brief Determine if an appender requires a layout.
     *
     * Configurators may call this method to determine if an appender requires
     * a layout.
     *
     * In the exceptional case where an appender accepts a layout but can also
     * work without it the appender should return \e TRUE.
     *
     * \param self [in] An appender object.
     *
     * \return \e TRUE if \e self requires a layout, \e FALSE otherwise.
     *
     * \see log4g/layout.h
     */
    gboolean
    (*requires_layout)(Log4gAppender *self);
};

GType
log4g_appender_get_type(void);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::add_filter().
 *
 * \param self [in] An appender object.
 * \param filter [in] A filter to add to \e self.
 *
 * \see log4g/filter.h
 */
void
log4g_appender_add_filter(Log4gAppender *self, Log4gFilter *filter);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::get_filter().
 *
 * \param self [in] An appender object.
 *
 * \return The first filter in the filter chain.
 *
 * \see log4g/filter.h
 */
Log4gFilter *
log4g_appender_get_filter(Log4gAppender *self);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::close().
 *
 * \param self [in] An appender object.
 */
void
log4g_appender_close(Log4gAppender *self);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::do_append().
 *
 * \param self [in] An appender object.
 * \param event [in] The log event to append.
 *
 * \see log4g/logging-event.h
 */
void
log4g_appender_do_append(Log4gAppender *self, Log4gLoggingEvent *event);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::get_name().
 *
 * \param self [in] An appender object.
 *
 * \return The name of \e self (may be \e NULL).
 */
const gchar *
log4g_appender_get_name(Log4gAppender *self);

/**
 * \brief Invokes the virtual function
 *        _Log4gAppenderInterface::set_error_handler().
 *
 * \param self [in] An appender object.
 * \param handler [in] The new error handler object for \e self.
 *
 * \see log4g/interface/error-handler.h
 */
void
log4g_appender_set_error_handler(Log4gAppender *self, gpointer handler);

/**
 * \brief Invokes the virtual function
 *        _Log4gAppenderInterface::get_error_handler().
 *
 * \param self [in] An appender object.
 *
 * \return The error handler object used by \e self.
 *
 * \see log4g/interface/error-handler.h
 */
gpointer
log4g_appender_get_error_handler(Log4gAppender *self);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::set_layout().
 *
 * \param self [in] An appender object.
 * \param layout [in] The new layout for \e self.
 */
void
log4g_appender_set_layout(Log4gAppender *self, Log4gLayout *layout);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::get_layout().
 *
 * \param self [in] An appender object.
 *
 * \return The layout set for \e self.
 */
Log4gLayout *
log4g_appender_get_layout(Log4gAppender *self);

/**
 * \brief Invokes the virtual function _Log4gAppenderInterface::set_name().
 *
 * \param self [in] An appender object.
 * \param name [in] The new name for \e self.
 */
void
log4g_appender_set_name(Log4gAppender *self, const gchar *name);

/**
 * \brief Invokes the virtual function
 *        _Log4gAppenderInterface::requires_layout().
 *
 * \param self [in] An appender object.
 */
gboolean
log4g_appender_requires_layout(Log4gAppender *self);

/**
 * \brief Invokes the virtual function
 *        _Log4gOptionHandlerInterface::activate_options().
 *
 * \param self [in] An appender object.
 */
void
log4g_appender_activate_options(Log4gAppender *self);

G_END_DECLS

#endif /* LOG4G_APPENDER_H */
