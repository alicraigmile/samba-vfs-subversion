/* 
 * Skeleton VFS module.
 *
 * Copyright (C) Tim Potter, 1999
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: vfs_tutorial.html,v 1.4 2001/07/23 15:33:19 davecb Exp $
 */

#include "config.h"

#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_UTIME_H
#include <utime.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <errno.h>
#include <string.h>

#include <vfs.h>

/* Function prototypes */

int skel_connect(struct vfs_connection_struct *conn, char *svc, char *user);
void skel_disconnect(void);
SMB_BIG_UINT skel_disk_free(char *path, SMB_BIG_UINT *bsize, 
			    SMB_BIG_UINT *dfree, SMB_BIG_UINT *dsize);

DIR *skel_opendir(char *fname);
struct dirent *skel_readdir(DIR *dirp);
int skel_mkdir(char *path, mode_t mode);
int skel_rmdir(char *path);
int skel_closedir(DIR *dir);

int skel_open(char *fname, int flags, mode_t mode);
int skel_close(int fd);
ssize_t skel_read(int fd, char *data, size_t n);
ssize_t skel_write(int fd, char *data, size_t n);
SMB_OFF_T skel_lseek(int filedes, SMB_OFF_T offset, int whence);
int skel_rename(char *old, char *new);
void skel_sync(int fd);
int skel_stat(char *fname, SMB_STRUCT_STAT *sbuf);
int skel_fstat(int fd, SMB_STRUCT_STAT *sbuf);
int skel_lstat(char *path, SMB_STRUCT_STAT *sbuf);
BOOL skel_lock(int fd, int op, SMB_OFF_T offset, SMB_OFF_T count, int type);
int skel_unlink(char *path);
int skel_chmod(char *path, mode_t mode);
int skel_utime(char *path, struct utimbuf *times);

/* VFS operations structure */

struct vfs_ops skel_ops = {

    /* Disk operations */

    skel_connect,
    skel_disconnect,
    skel_disk_free,

    /* Directory operations */

    skel_opendir,
    skel_readdir,
    skel_mkdir,
    skel_rmdir,
    skel_closedir,

    /* File operations */

    skel_open,
    skel_close,
    skel_read,
    skel_write,
    skel_lseek,
    skel_rename,
    skel_sync,
    skel_stat,
    skel_fstat,
    skel_lstat,
    skel_lock,
    skel_unlink,
    skel_chmod,
    skel_utime
};

/* VFS initialization - return vfs_ops function pointer structure */


extern struct vfs_ops default_vfs_ops; /* For passthrough operation */

struct vfs_ops *vfs_init(struct vfs_ops *former)
{
    
    return(&skel_ops);
}



/* Implementation of VFS functions.  Insert your useful stuff here */


int skel_connect(struct vfs_connection_struct *conn, char *svc, char *user)
{
    return underlying.connect(conn, svc, user);
}

void skel_disconnect(void)
{
    underlying.disconnect();
}

SMB_BIG_UINT skel_disk_free(char *path, SMB_BIG_UINT *bsize, 
			    SMB_BIG_UINT *dfree, SMB_BIG_UINT *dsize)
{
    return underlying.disk_free(path, bsize, dfree, dsize);
}

DIR *skel_opendir(char *fname)
{
    return underlying.opendir(fname);
}

struct dirent *skel_readdir(DIR *dirp)
{
    return underlying.readdir(dirp);
}

int skel_mkdir(char *path, mode_t mode)
{
    return underlying.mkdir(path, mode);
}

int skel_rmdir(char *path)
{
    return underlying.rmdir(path);
}

int skel_closedir(DIR *dir)
{
    return underlying.closedir(dir);
}

int skel_open(char *fname, int flags, mode_t mode)
{
    return underlying.open(fname, flags, mode);
}

int skel_close(int fd)
{
    return underlying.close(fd);
}

ssize_t skel_read(int fd, char *data, size_t n)
{
    return underlying.read(fd, data, n);
}

ssize_t skel_write(int fd, char *data, size_t n)
{
    return underlying.write(fd, data, n);
}

SMB_OFF_T skel_lseek(int filedes, SMB_OFF_T offset, int whence)
{
    return underlying.lseek(filedes, offset, whence);
}

int skel_rename(char *old, char *new)
{
    return underlying.rename(old, new);
}

void skel_sync(int fd)
{
    underlying.sync(fd);
}

int skel_stat(char *fname, SMB_STRUCT_STAT *sbuf)
{
    return underlying.stat(fname, sbuf);
}

int skel_fstat(int fd, SMB_STRUCT_STAT *sbuf)
{
    return underlying.fstat(fd, sbuf);
}

int skel_lstat(char *path, SMB_STRUCT_STAT *sbuf)
{
    return underlying.lstat(path, sbuf);
}

BOOL skel_lock(int fd, int op, SMB_OFF_T offset, SMB_OFF_T count, int type)
{
    return underlying.lock(fd, op, offset, count, type);
}

int skel_unlink(char *path)
{
    return underlying.unlink(path);
}

int skel_chmod(char *path, mode_t mode)
{
    return underlying.chmod(path, mode);
}

int skel_utime(char *path, struct utimbuf *times)
{
    return underlying.utime(path, times);
}

