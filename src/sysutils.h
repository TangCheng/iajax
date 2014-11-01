/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * sysutils.h
 * Copyright (C) 2014 Watson Xu <xuhuashan@gmail.com>
 *
 * iconfig is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * iconfig is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SYSUTILS_H_
#define _SYSUTILS_H_

gboolean sysutils_datetime_get_datetime(gchar **str_value);
gint sysutils_get_upgrade_status(void);
gboolean sysutils_get_uptime(gchar **uptime);
gboolean sysutils_get_cpu_info(gchar **cpuinfo);
guint sysutils_get_cpu_usage(void);
gboolean sysutils_get_memory_info(gchar **total, gchar **used_mem, gchar **free_mem, guint *usage);
gboolean sysutils_get_net_info(gchar **band_width, gchar **sent, gchar **recved, guint *tx_usage, guint *rx_usage);

#endif // _SYSUTILS_H_

