## Process this file with automake to produce Makefile.in

## Created by Anjuta

AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPACKAGE_SYSCONFDIR=\""$(sysconfdir)"\" \
	$(IAJAX_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bindir = $(prefix)/iajax
bin_PROGRAMS = iajax

iajax_SOURCES = \
	ajax/http_response.c \
	ajax/http_request.c \
	ajax/http_proc.c \
	ajax/http_parser.c \
	ajax/http_query_string_parser.c \
	ajax/ajax.c \
	ajax/ctemplate.c \
	http_handler/request_handler.c \
	http_handler/configuration/misc_handler.c \
	http_handler/configuration/base_info_handler.c \
	http_handler/configuration/users_handler.c \
	http_handler/configuration/datetime_handler.c \
	http_handler/configuration/video_handler.c \
	http_handler/configuration/image_handler.c \
	http_handler/configuration/privacy_block_handler.c \
	http_handler/configuration/day_night_mode_handler.c \
	http_handler/configuration/osd_handler.c \
	http_handler/configuration/szyc_handler.c \
	http_handler/configuration/network_handler.c \
	http_handler/configuration/event_input_handler.c \
	http_handler/configuration/event_output_handler.c \
	http_handler/configuration/event_motion_handler.c \
	http_handler/configuration/event_cover_handler.c \
	http_handler/configuration/event_proc_handler.c \
	http_handler/login/login_handler.c \
	http_handler/login/logout_handler.c \
	http_handler/upgrade/upgrade_handler.c \
	http_handler/system/sysinfo_handler.c \
	login/login_manager.c \
	login/sha1.c \
	iajax.c \
	common.c \
	sysutils.c \
	event-handler.c \
	main.c

iajax_LDFLAGS = 

iajax_LDADD = $(IAJAX_LIBS) -lhttp_parser


SUBDIRS = \
	config
