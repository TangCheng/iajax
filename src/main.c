/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2014 TangCheng <tangcheng_@163.com>
 * 
 */

#include "iajax.h"
#include <stdio.h>

int main()
{
	IpcamIAjax *iajax = g_object_new(IPCAM_IAJAX_TYPE, "name", "iajax", NULL);
	ipcam_base_service_start(IPCAM_BASE_SERVICE(iajax));
	ipcam_base_service_stop(IPCAM_BASE_SERVICE(iajax));
	g_object_run_dispose(G_OBJECT(iajax));
	g_object_unref(iajax);

	return (0);
}

