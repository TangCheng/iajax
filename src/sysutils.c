/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * sysutils.c
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

#define _GNU_SOURCE
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <errno.h>
#include <glib.h>
#include "sysutils.h"

#define SECONDS_OF_MIN   (60)
#define SECONDS_OF_HOUR  (SECONDS_OF_MIN * 60)
#define SECONDS_OF_DAY   (SECONDS_OF_HOUR * 24)

gboolean sysutils_datetime_get_datetime(gchar **str_value)
{
    time_t t;
    struct tm tm1;
    gchar buf[32];

    if (str_value) {
        t = time(NULL);
        if (localtime_r(&t, &tm1)) {
            strftime(buf, sizeof(buf), "%F %T", &tm1);
            *str_value = g_strdup(buf);

            return TRUE;
        }
        *str_value = NULL;
    }

    return FALSE;
}

gint sysutils_get_upgrade_status(void)
{
    FILE *fp = NULL;
    gint ret = 0;

    if (0 == access("/var/cache/firmware/firmware-lock", F_OK))
    {
        fp = popen("cat /var/cache/firmware/firmware-lock", "r");
        if (fp) {
            fscanf(fp, "%d", &ret);
            pclose(fp);
        }
    }

    return ret;
}

gboolean sysutils_get_uptime(gchar **uptime)
{
    gchar buf[64];
    struct sysinfo s_info;
    time_t now;
    gboolean ret = FALSE;

    if (uptime && (0 == sysinfo(&s_info)))
    {
        sysinfo(&s_info);
        time(&now);
        now -= s_info.uptime;

        guint64 day = now / SECONDS_OF_DAY;
        guint64 hour = (now % SECONDS_OF_DAY) / SECONDS_OF_HOUR;
        guint64 min = ((now % SECONDS_OF_DAY) % SECONDS_OF_HOUR) / SECONDS_OF_MIN;
    
        if (day == 0)
        {
            g_snprintf(buf, 64, "%llu:%llu", hour, min);
        }
        else if (day == 1)
        {
            g_snprintf(buf, 64, "1 day, %llu:%llu", hour, min);
        }
        else
        {
            g_snprintf(buf, 64, "%llu days, %llu:%llu", day, hour, min);
        }
        
        *uptime = g_strdup(buf);
        ret = TRUE;
    }

    return ret;
}

gboolean sysutils_get_cpu_info(gchar **cpuinfo)
{
    FILE *fp = NULL;
    gchar buf[64];
    gboolean ret = FALSE;
    
    fp = popen("cat /proc/cpuinfo | awk -F: \'{print $2}\' | sed \'s/\\ //g\'", "r");
    if (fp) {
        fscanf(fp, "%s", buf);
        g_strlcat(buf, "MHz", 64);
        *cpuinfo = g_strdup(buf);
        ret = TRUE;
        pclose(fp);
    }
    
    return ret;
}

gboolean sysutils_get_cpu_usage(gchar **usage)
{
}
