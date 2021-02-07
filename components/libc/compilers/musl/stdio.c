/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017/10/15     bernard      the first version
 */
#include <stdio.h>
#include <stdlib.h>

#include <rtthread.h>
#include "libc.h"
#include "dfs.h"

#define STDIO_DEVICE_NAME_MAX   32

int	_EXFUN(fileno, (FILE *));

static FILE* std_console = NULL;

int libc_stdio_set_console(const char* device_name, int mode)
{
    FILE *fp;
    char name[STDIO_DEVICE_NAME_MAX];
    char *file_mode;

    snprintf(name, sizeof(name) - 1, "/dev/%s", device_name);
    name[STDIO_DEVICE_NAME_MAX - 1] = '\0';

    if (mode == O_RDWR) file_mode = "r+";
    else if (mode == O_WRONLY) file_mode = "wb";
    else file_mode = "rb";

    fp = fopen(name, file_mode);
    if (fp)
    {
        setvbuf(fp, NULL, _IONBF, 0);

        if (std_console)
        {
            fclose(std_console);
            std_console = NULL;
        }
        std_console = fp;
    }

    if (std_console)
    {
        struct dfs_fdtable *fdt = dfs_fdtable_get();
        int fd = fileno(std_console);

        /* set fd (0, 1, 2) */
        fd_associate(fdt, 0, fd_get(fd));
        fd_associate(fdt, 1, fd_get(fd));
        fd_associate(fdt, 2, fd_get(fd));
        return fd;
    }

    return -1;
}

int libc_stdio_get_console(void) {
    if (std_console)
        return fileno(std_console);
    else
        return -1;
}
