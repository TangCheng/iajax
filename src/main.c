/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2014 TangCheng <tangcheng_@163.com>
 * 
 */

#include "iajax.h"

int main()
{
	IpcamIAjax *iajax = g_object_new(IPCAM_IAJAX_TYPE, "name", "iajax", NULL);
	ipcam_base_service_start(IPCAM_BASE_SERVICE(iajax));
	return (0);
}

