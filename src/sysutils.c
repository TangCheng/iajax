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
#include <time.h>
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
    gboolean ret = FALSE;

    if (uptime && (0 == sysinfo(&s_info)))
    {
        guint32 day = s_info.uptime / SECONDS_OF_DAY;
        guint32 hour = (s_info.uptime % SECONDS_OF_DAY) / SECONDS_OF_HOUR;
        guint32 min = ((s_info.uptime % SECONDS_OF_DAY) % SECONDS_OF_HOUR) / SECONDS_OF_MIN;
    
        if (day == 0)
        {
            g_snprintf(buf, 64, "%lu:%lu", hour, min);
        }
        else if (day == 1)
        {
            g_snprintf(buf, 64, "1 day, %lu:%lu", hour, min);
        }
        else
        {
            g_snprintf(buf, 64, "%lu days, %lu:%lu", day, hour, min);
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
    guint freq = 0;
    gboolean ret = FALSE;
    
    fp = popen("cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", "r");
    if (fp) {
        fscanf(fp, "%u", &freq);
        g_snprintf(buf, 64, "%uMHz", freq / 1000);
        *cpuinfo = g_strdup(buf);
        ret = TRUE;
        pclose(fp);
    }
    
    return ret;
}

guint sysutils_get_cpu_usage(void)
{
    guint usage = 0;

    FILE *fp = popen("cat /proc/stat", "r");
    if (fp == NULL) {
        perror("error get cpu usage");
        return usage;
    }

    /* 
     * Output for example
     * cpu  672316 0 43292 158 0 0 101 0 0 0
     */
    static guint64 old_user = 0, old_nic = 0, old_sys = 0, old_idle = 0, old_io = 0, old_irq = 0, old_sirq = 0;
    guint64 user, nic, sys, idle, io, irq, sirq;
    while (!feof(fp))
    {
        char buf[256];
        if (fgets(buf, sizeof(buf), fp) == NULL)
            break;
        if (sscanf(buf, "cpu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nic, &sys, &idle, &io, &irq, &sirq) == 7)
        {
            guint64 total_idle = idle - old_idle;
            guint64 total = (user + nic + sys + idle + io + irq + sirq) -
                (old_user + old_nic + old_sys + old_idle + old_io + old_irq + old_sirq);
            
            usage = 100 - ((total_idle ) * 100 / total);
            old_user = user;
            old_nic = nic;
            old_sys = sys;
            old_idle = idle;
            old_io = io;
            old_irq = irq;
            old_sirq = sirq;
            break;
        }
    }

    pclose(fp);

    return usage;
}

gboolean sysutils_get_memory_info(gchar **total, gchar **used_mem, gchar **free_mem, guint *usage)
{
    gchar buf[64];
    struct sysinfo s_info;
    gboolean ret = FALSE;

    if (total && used_mem && free_mem && usage && (0 == sysinfo(&s_info)))
    {
        g_snprintf(buf, 64, "%luKB", s_info.totalram / 1024);
        *total = g_strdup(buf);
        g_snprintf(buf, 64, "%luKB", (s_info.totalram - s_info.freeram) / 1024);
        *used_mem = g_strdup(buf);
        g_snprintf(buf, 64, "%luKB", s_info.freeram  / 1024);
        *free_mem = g_strdup(buf);
        *usage = (s_info.totalram - s_info.freeram) * (guint64)100 / s_info.totalram;
        
        ret = TRUE;
    }

    return ret;
}

gboolean sysutils_get_net_info(gchar **band_width, gchar **sent, gchar **recved, guint *tx_usage, guint *rx_usage)
{
    gchar buf[64];
    gboolean ret = FALSE;
    FILE *fp;
    static guint64 old_tx = 0;
    static guint64 old_rx = 0;
    static time_t old_time = 0;
    guint64 tx;
    guint64 rx;
    guint bw = 1000;

    if (band_width && sent && recved && tx_usage && rx_usage)
    {
        fp = popen("cat /sys/class/net/eth0/speed", "r");
        if (fp) {
            fscanf(fp, "%u", &bw);
            g_snprintf(buf, 64, "%uMbps", bw);
            *band_width = g_strdup(buf);
            pclose(fp);
        }

        fp = popen("cat /sys/class/net/eth0/statistics/tx_bytes", "r");
        if (fp) {
            fscanf(fp, "%llu", &tx);
            if (tx > 1024 * 1024 * 1024LL)
            {
                g_snprintf(buf, 64, "%.01fGB", tx / 1024 / 1024 / 1024.0);
            }
            else if (tx > 1024 * 1024LL)
            {
                g_snprintf(buf, 64, "%.01fMB", tx / 1024 / 1024.0);
            }
            else if (tx > 1024LL)
            {
                g_snprintf(buf, 64, "%.01fKB", tx / 1024.0);
            }
            else
            {
                g_snprintf(buf, 64, "%lluBytes", tx);
            }
            
            *sent = g_strdup(buf);
            pclose(fp);
        }

        fp = popen("cat /sys/class/net/eth0/statistics/rx_bytes", "r");
        if (fp) {
            fscanf(fp, "%llu", &rx);
            if (rx > 1024LL * 1024 * 1024)
            {
                g_snprintf(buf, 64, "%.01fGB", rx / 1024 / 1024 / 1024.0);
            }
            else if (rx > 1024LL * 1024)
            {
                g_snprintf(buf, 64, "%.01fMB", rx / 1024 / 1024.0);
            }
            else if (rx > 1024LL)
            {
                g_snprintf(buf, 64, "%.01fKB", rx / 1024.0);
            }
            else
            {
                g_snprintf(buf, 64, "%lluBytes", rx);
            }
            *recved = g_strdup(buf);
            pclose(fp);
        }

        time_t now;
        time(&now);
        *tx_usage = 0;
        *rx_usage = 0;
        if (now > old_time)
        {
            if (tx > old_tx)
            {
                *tx_usage = (tx - old_tx) * 8LL * 100 / (now - old_time) / 1024 / 1024 / bw;
                old_tx = tx;
            }

            if (rx > old_rx)
            {
                *rx_usage = (rx - old_rx) * 8LL * 100 / (now - old_time) / 1024 / 1024 / bw;
                old_rx = rx;
            }
        }

        old_time = now;

        ret = TRUE;
    }

    return ret;
}

void sysutils_reboot(void)
{
    system("reboot");
}

void sysutils_soft_reset(void)
{
    system("/apps/iconfig/soft-reset.sh");
}

void sysutils_hard_reset(void)
{
    system("/apps/iconfig/hard-reset.sh");
}
