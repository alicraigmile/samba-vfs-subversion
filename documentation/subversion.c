/* 
 * Subversion VFS module.
 *
 * Copyright (C) Ali Craigmile, 2009
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

int svn_connect(struct vfs_connection_struct *conn, char *svc, char *user);
void svn_disconnect(void);
SMB_BIG_UINT svn_disk_free(char *path, SMB_BIG_UINT *bsize, 
			    SMB_BIG_UINT *dfree, SMB_BIG_UINT *dsize);

DIR *svn_opendir(char *fname);
struct dirent *svn_readdir(DIR *dirp);
int svn_mkdir(char *path, mode_t mode);
int svn_rmdir(char *path);
int svn_closedir(DIR *dir);

int svn_open(char *fname, int flags, mode_t mode);
int svn_close(int fd);
ssize_t svn_read(int fd, char *data, size_t n);
ssize_t svn_write(int fd, char *data, size_t n);
SMB_OFF_T svn_lseek(int filedes, SMB_OFF_T offset, int whence);
int svn_rename(char *old, char *new);
void svn_sync(int fd);
int svn_stat(char *fname, SMB_STRUCT_STAT *sbuf);
int svn_fstat(int fd, SMB_STRUCT_STAT *sbuf);
int svn_lstat(char *path, SMB_STRUCT_STAT *sbuf);
BOOL svn_lock(int fd, int op, SMB_OFF_T offset, SMB_OFF_T count, int type);
int svn_unlink(char *path);
int svn_chmod(char *path, mode_t mode);
int svn_utime(char *path, struct utimbuf *times);

/* VFS operations structure */

struct vfs_ops svn_ops = {

    /* Disk operations */

    svn_connect,
    svn_disconnect,
    svn_disk_free,

    /* Directory operations */

    svn_opendir,
    svn_readdir,
    svn_mkdir,
    svn_rmdir,
    svn_closedir,

    /* File operations */

    svn_open,
    svn_close,
    svn_read,
    svn_write,
    svn_lseek,
    svn_rename,
    svn_sync,
    svn_stat,
    svn_fstat,
    svn_lstat,
    svn_lock,
    svn_unlink,
    svn_chmod,
    svn_utime
};

/* VFS initialization - return vfs_ops function pointer structure */


extern struct vfs_ops default_vfs_ops; /* For passthrough operation */

struct vfs_ops *vfs_init(struct vfs_ops *former)
{
    
    return(&svn_ops);
}



/* Implementation of VFS functions.  Insert your useful stuff here */


int svn_connect(struct vfs_connection_struct *conn, char *svc, char *user)
{
    return underlying.connect(conn, svc, user);
}

void svn_disconnect(void)
{
    underlying.disconnect();
}

SMB_BIG_UINT svn_disk_free(char *path, SMB_BIG_UINT *bsize, 
			    SMB_BIG_UINT *dfree, SMB_BIG_UINT *dsize)
{
    return underlying.disk_free(path, bsize, dfree, dsize);
}

DIR *svn_opendir(char *fname)
{
    return underlying.opendir(fname);
}

struct dirent *svn_readdir(DIR *dirp)
{
    return underlying.readdir(dirp);
}

int svn_mkdir(char *path, mode_t mode)
{
    return underlying.mkdir(path, mode);
}

int svn_rmdir(char *path)
{
    return underlying.rmdir(path);
}

int svn_closedir(DIR *dir)
{
    return underlying.closedir(dir);
}

int svn_open(char *fname, int flags, mode_t mode)
{
    return underlying.open(fname, flags, mode);
}

int svn_close(int fd)
{
    return underlying.close(fd);
}

ssize_t svn_read(int fd, char *data, size_t n)
{
    return underlying.read(fd, data, n);
}

ssize_t svn_write(int fd, char *data, size_t n)
{
    return underlying.write(fd, data, n);
}

SMB_OFF_T svn_lseek(int filedes, SMB_OFF_T offset, int whence)
{
    return underlying.lseek(filedes, offset, whence);
}

int svn_rename(char *old, char *new)
{
    return underlying.rename(old, new);
}

void svn_sync(int fd)
{
    underlying.sync(fd);
}

int svn_stat(char *fname, SMB_STRUCT_STAT *sbuf)
{
    return underlying.stat(fname, sbuf);
}

int svn_fstat(int fd, SMB_STRUCT_STAT *sbuf)
{
    return underlying.fstat(fd, sbuf);
}

int svn_lstat(char *path, SMB_STRUCT_STAT *sbuf)
{
    return underlying.lstat(path, sbuf);
}

BOOL svn_lock(int fd, int op, SMB_OFF_T offset, SMB_OFF_T count, int type)
{
    return underlying.lock(fd, op, offset, count, type);
}

int svn_unlink(char *path)
{
    return underlying.unlink(path);
}

int svn_chmod(char *path, mode_t mode)
{
    return underlying.chmod(path, mode);
}

int svn_utime(char *path, struct utimbuf *times)
{
    return underlying.utime(path, times);
}

