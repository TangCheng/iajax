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
#include <glib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <errno.h>
#include "sysutils.h"

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

gboolean sysutils_datetime_set_datetime(gchar *str_value)
{
    time_t timer;
    struct tm tm;

    timer = time(NULL);
    localtime_r(&timer, &tm);
    if (sscanf(str_value, "%d-%d-%d %d:%d:%d",
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec) < 6)
        return FALSE;

    tm.tm_year -= 1900;
    tm.tm_mon--;
    timer = mktime(&tm);
    if (timer == (time_t)-1)
        return FALSE;

    return(stime(&timer) == 0);
}


int sysutils_make_directory(char *path, long mode)
{
	mode_t cur_mask;
	mode_t org_mask;
	const char *fail_msg;
	char *s;
	char c;
	struct stat st;
	int retval = 0;

#define LONE_CHAR(s,c)     ((s)[0] == (c) && !(s)[1])
	if (LONE_CHAR(path, '.'))
		return 0;

	org_mask = cur_mask = (mode_t)-1L;
	s = path;
	while (1) {
		c = '\0';

		/* Bypass leading non-'/'s and then subsequent '/'s */
		while (*s) {
			if (*s == '/') {
				do {
					++s;
				} while (*s == '/');
				c = *s; /* Save the current char */
				*s = '\0'; /* and replace it with nul */
				break;
			}
			++s;
		}

		if (c != '\0') {
			/* Intermediate dirs: must have wx for user */
			if (cur_mask == (mode_t)-1L) { /* wasn't done yet? */
				mode_t new_mask;
				org_mask = umask(0);
				cur_mask = 0;
				/* Clear u=wx in umask - this ensures
				 * they won't be cleared on mkdir */
				new_mask = (org_mask & ~(mode_t)0300);
				//bb_error_msg("org_mask:%o cur_mask:%o", org_mask, new_mask);
				if (new_mask != cur_mask) {
					cur_mask = new_mask;
					umask(new_mask);
				}
			}
		} else {
			/* Last component: uses original umask */
			//bb_error_msg("1 org_mask:%o", org_mask);
			if (org_mask != cur_mask) {
				cur_mask = org_mask;
				umask(org_mask);
			}
		}

		if (mkdir(path, 0777) < 0) {
			/* If we failed for any other reason than the directory
			 * already exists, output a diagnostic and return -1 */
			if (errno != EEXIST
			 || ((stat(path, &st) < 0) || !S_ISDIR(st.st_mode))
			) {
				fail_msg = "create";
				break;
			}
			/* Since the directory exists, don't attempt to change
			 * permissions if it was the full target.  Note that
			 * this is not an error condition. */
			if (!c) {
				goto ret;
			}
		}

		if (!c) {
			/* Done.  If necessary, update perms on the newly
			 * created directory.  Failure to update here _is_
			 * an error. */
			if ((mode != -1) && (chmod(path, mode) < 0)) {
				fail_msg = "set permissions of";
				break;
			}
			goto ret;
		}

		/* Remove any inserted nul from the path (recursive mode) */
		*s = c;
	} /* while (1) */

	fprintf(stderr, "can't %s directory '%s'", fail_msg, path);
	retval = -1;
 ret:
	//bb_error_msg("2 org_mask:%o", org_mask);
	if (org_mask != cur_mask)
		umask(org_mask);
	return retval;
}


int sysutils_network_get_interfaces(char **ifnames, int *nr)
{
    FILE *fp;
    int ifs = 0;

    fp = popen("ip -o -0 addr show", "r");
    if (fp == NULL) {
        perror("error get network_interface");
        return -1;
    }

    /* 
     * Output for example
     * 2: eth0    inet 192.168.10.15/24 brd 192.168.10.255 scope global eth0
     */
    while (!feof(fp) && ifs < *nr) {
        char buf[256];
        char name[32];
        if (fgets(buf, sizeof(buf), fp) == NULL)
            break;
        if (sscanf(buf, "%*d:%s %*[^$] %*[$]", name) == 1)
        {
            ifnames[ifs++] = strdup(name);
        }
    }
    *nr = ifs;
    pclose(fp);

    return 0;
}

int sysutils_network_if_indextoname(unsigned int ifindex, char *ifname)
{
    int sockfd;
    struct ifreq ifr;
    int ret;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return -1;

    ifr.ifr_ifindex = ifindex;
    ret = ioctl(sockfd, SIOCGIFNAME, &ifr);

    if (ret == 0)
        strncpy(ifname, ifr.ifr_name, IFNAMSIZ);

    close(sockfd);

    return 0;
}

static char *__prefixlen2_mask(unsigned int prefix_len)
{
	struct in_addr addr = { 0 };
	int i;

	for (i = 31; i >= 32 - (int)prefix_len; i--)
		addr.s_addr |= (1 << i);
    addr.s_addr = htonl(addr.s_addr);
	return inet_ntoa(addr);
}

int sysutils_network_set_hwaddr(const char *ifname, const char *hwaddr)
{
    char *cmd;
    FILE *fp;

    asprintf(&cmd, "ifconfig %s hw ether %s", ifname, hwaddr);
    fp = popen(cmd, "r");
    free(cmd);
    if (fp == NULL)
        return -1;

    pclose(fp);

    return 0;
}

int sysutils_network_get_hwaddr(const char *ifname, char **hwaddr)
{
    char *cmd;
    FILE *fp;
    int ret = -1;

    asprintf(&cmd, "ifconfig %s | grep HWaddr | awk \'{print $5}\'", ifname);
    fp = popen(cmd, "r");
    free(cmd);
    if (fp == NULL)
        return ret;

    *hwaddr = calloc(18, sizeof(char));
    ret = fread(*hwaddr, sizeof(char), 17, fp);
    pclose(fp);
    
    return ret;
}

int sysutils_network_get_address(const char *ifname,
                                 char **ipaddr,
                                 char **netmask,
                                 char **broadaddr)
{
    char *cmd;
    FILE *fp;
    int ret = -1;

    asprintf(&cmd, "ip -o -4 addr show dev %s", ifname);
    fp = popen(cmd, "r");
    free(cmd);

    if (fp) {
        char ip[32];
        guint mask;
        char brd[32];

        /* 
         * Output for example
         * 2: eth0    inet 192.168.10.15/24 brd 192.168.10.255 scope global eth0
         */
        if (fscanf(fp, "%*d:%*s inet %32[0-9.] %*c %d brd %32[0-9.] %*[^$] %*[$]",
                   ip, &mask, brd) == 3)
        {
            if (ipaddr)
                *ipaddr = strdup(ip);
            if (netmask)
                *netmask = strdup(__prefixlen2_mask(mask));
            if (broadaddr)
                *broadaddr = strdup(brd);

            ret = 0;
        }
        pclose(fp);
    }

    return ret;
}

int sysutils_network_set_address(const char *ifname,
                                 const char *ipaddr,
                                 const char *netmask,
                                 const char *broadaddr)
{
    char *cmd = NULL;
    char *_brd = NULL;
    char *_mask = NULL;
    FILE *fp;
    int ret = -1;

    if (broadaddr)
        asprintf(&_brd, "-broadcast %s", broadaddr);
    if (netmask)
        asprintf(&_mask, "netmask %s", netmask);

    asprintf(&cmd, "ifconfig %s %s %s %s", ifname,
             ipaddr ? ipaddr : "",
             netmask ? _mask : "",
             broadaddr ? _brd : "");
    fp = popen(cmd, "r");
    free(cmd);
    free(_brd);
    free(_mask);

    if (fp) {
        pclose(fp);

        ret = 0;
    }

    return ret;
}

int sysutils_network_get_gateway(const char *ifname, char **gwaddr)
{
    FILE *fp;
    int ret = -1;
    char buf[128];
    char gw[32];

    fp = popen("ip route", "r");
    if (fp == NULL)
        return -1;

        /* 
         * Output for example
         * default via 192.168.1.1 dev wlp8s0  proto static  metric 1024
         */
    while(!feof(fp)) {
        if (fgets(buf, sizeof(buf), fp) == NULL)
            continue;
        if (sscanf(buf, "default via %32s %*[^$] %*[$]", gw) == 1) {
            *gwaddr = strdup(gw);
            ret = 0;
            break;
        }
        pclose(fp);
    }

    return ret;
}

int sysutils_network_set_gateway(const char *ifname, const char *gwaddr)
{
    char *cmd;
    FILE *fp;

    /* Delete the route */
    fp = popen("ip route del default", "r");
    if (fp == NULL)
        return -1;
    pclose(fp);

    /* Add new route */
    asprintf(&cmd, "ip route add default via %s dev %s", gwaddr, ifname);
    fp = popen(cmd, "r");
    free(cmd);
    if (fp == NULL)
        return -1;

    pclose(fp);

    return 0;
}

int sysutils_network_get_dns(const char *ifname, char **dns, int *size)
{
    FILE *fp;
    char *fname = PACKAGE_SYSCONFDIR "/resolv.conf";
    char buf[128];
    int i;

    if (access(PACKAGE_SYSCONFDIR, F_OK) < 0)
        mkdir(PACKAGE_SYSCONFDIR, 0775);

    fp = fopen(fname, "r");
    if (fp == NULL)
        return -1;

    i = 0;
    while(!feof(fp) && i < *size) {
        char val[32];

        if (fgets(buf, sizeof(buf), fp) == NULL)
            continue;

        if (sscanf(buf, "nameserver %32s", val) == 1) {
            dns[i] = strdup(val);
            i++;
        }
    }
    *size = i;

    fclose(fp);

    return 0;
}

int sysutils_network_set_dns(const char *ifname, const char **dns, int size)
{
    FILE *fp;
    char buf[128];
    int i;

    if (access(PACKAGE_SYSCONFDIR, F_OK) < 0)
        mkdir(PACKAGE_SYSCONFDIR, 0775);

    fp = fopen(PACKAGE_SYSCONFDIR "/resolv.conf", "w+");
    if (fp == NULL)
        return -1;

    fputs("# Generated by IConfig\n", fp);
    for (i = 0; i < size; i++) {
        if (dns[i] == NULL || strlen(dns[i]) == 0)
            continue;
        snprintf(buf, sizeof(buf), "nameserver %s\n", dns[i]);
        fputs(buf, fp);
    }

    fclose(fp);
    return i;
}

int sysutils_network_get_hostname(const char **hostname)
{
    FILE *fp = NULL;
    char buf[64];
    int ret = -1;

    fp = popen("hostname", "r");
    if (fp) {
        if (fscanf(fp, "%64s", buf)) {
            *hostname = strdup(buf);
            ret = 0;
        }
        pclose(fp);
    }

    return ret;
}

int sysutils_network_set_hostname(const char *hostname)
{
    FILE *fp = NULL;
    char cmd[64];
    int ret = -1;

    snprintf(cmd, sizeof(cmd), "hostname %s", hostname);
    fp = popen(cmd, "r");
    if (fp) {
        pclose(fp);
        ret = 0;
    }

    return ret;
}
