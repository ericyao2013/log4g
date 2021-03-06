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

/*
 * Tests for Log4gLevel
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "log4g/log4g.h"

#define CLASS "/log4g/Level"

void
test_001(G_GNUC_UNUSED gpointer *fixture, G_GNUC_UNUSED gconstpointer data)
{
	Log4gLevel *warn = log4g_level_WARN();
	g_assert(warn);
	Log4gLevel *error = log4g_level_string_to_level("ERROR");
	g_assert(error);
	Log4gLevel *level = log4g_level_int_to_level(log4g_level_to_int(warn));
	g_assert(level);
	g_assert(log4g_level_equals(warn, level));
	g_assert(!log4g_level_equals(warn, error));
	g_assert(!log4g_level_is_greater_or_equal(warn, error));
	g_assert(log4g_level_is_greater_or_equal(error, warn));
}

void
test_002(G_GNUC_UNUSED gpointer *fixture, G_GNUC_UNUSED gconstpointer data)
{
	Log4gLevel *level = log4g_level_new(10, "FOO", 2);
	g_assert(level);
	g_assert_cmpint(log4g_level_to_int(level), ==, 10);
	g_assert_cmpstr(log4g_level_to_string(level), ==, "FOO");
	g_assert_cmpint(log4g_level_get_syslog_equivalent(level), ==, 2);
	g_assert(log4g_level_is_greater_or_equal(level, level));
	g_assert(log4g_level_equals(level, level));
	g_object_unref(level);
}

int
main(int argc, char *argv[])
{
	g_test_init(&argc, &argv, NULL);
#if !GLIB_CHECK_VERSION(2, 36, 0)
	g_type_init();
#endif
	g_test_add(CLASS"/001", gpointer, NULL, NULL, test_001, NULL);
	g_test_add(CLASS"/002", gpointer, NULL, NULL, test_002, NULL);
	return g_test_run();
}
