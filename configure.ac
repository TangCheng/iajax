dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(iajax, 0.1.00)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

AM_PROG_CC_C_O

AC_USE_SYSTEM_EXTENSIONS


PKG_CHECK_MODULES(IAJAX, [zlib libzmq libczmq libffi gobject-2.0 gmodule-2.0 json-glib-1.0 gio-2.0 yaml-0.1 libipcam_base-0.1.0])


AC_OUTPUT([
Makefile
src/Makefile
src/config/Makefile

])
