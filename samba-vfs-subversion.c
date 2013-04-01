/* 
 * Subversion VFS module.  Implements passthrough operation of all VFS
 * calls to disk functions, however if share is an svn working copy,
 * commits all additions/changes/deletions automatically.
 *
 * Copyright (C) Ali Craigmile, 2011
 * Copyright (C) Tim Potter, 1999-2000
 * Copyright (C) Alexander Bokovoy, 2002
 * Copyright (C) Stefan (metze) Metzmacher, 2003
 * Copyright (C) Jeremy Allison 2009
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "includes.h"
#include <syslog.h>
#include <svn_types.h>
#include <svn_client.h>

#define vfs_svn_init init_samba_module

/* PLEASE,PLEASE READ THE VFS MODULES CHAPTER OF THE 
   SAMBA DEVELOPERS GUIDE!!!!!!
 */

/* If you take this file as template for your module
 * please make sure that you remove all vfs_svn_XXX() functions you don't
 * want to implement!! The passthrough operations are not
 * neccessary in a real module.
 *
 * --metze
 */

static int vfs_svn_connect(vfs_handle_struct *handle,  const char *service, const char *user)    
{
	return SMB_VFS_NEXT_CONNECT(handle, service, user);
}

static void vfs_svn_disconnect(vfs_handle_struct *handle)
{
	SMB_VFS_NEXT_DISCONNECT(handle);
}

static uint64_t vfs_svn_disk_free(vfs_handle_struct *handle,  const char *path,
	bool small_query, uint64_t *bsize,
	uint64_t *dfree, uint64_t *dsize)
{
	return SMB_VFS_NEXT_DISK_FREE(handle, path, small_query, bsize, 
					 dfree, dsize);
}

static int vfs_svn_get_quota(vfs_handle_struct *handle,  enum SMB_QUOTA_TYPE qtype, unid_t id, SMB_DISK_QUOTA *dq)
{
	return SMB_VFS_NEXT_GET_QUOTA(handle, qtype, id, dq);
}

static int vfs_svn_set_quota(vfs_handle_struct *handle,  enum SMB_QUOTA_TYPE qtype, unid_t id, SMB_DISK_QUOTA *dq)
{
	return SMB_VFS_NEXT_SET_QUOTA(handle, qtype, id, dq);
}

static int vfs_svn_get_shadow_copy_data(vfs_handle_struct *handle, files_struct *fsp, SHADOW_COPY_DATA *shadow_copy_data, bool labels)
{
	return SMB_VFS_NEXT_GET_SHADOW_COPY_DATA(handle, fsp, shadow_copy_data, labels);
}

static int vfs_svn_statvfs(struct vfs_handle_struct *handle, const char *path, struct vfs_statvfs_struct *statbuf)
{
	return SMB_VFS_NEXT_STATVFS(handle, path, statbuf);
}

static uint32_t vfs_svn_fs_capabilities(struct vfs_handle_struct *handle, enum timestamp_set_resolution *p_ts_res)
{
	return SMB_VFS_NEXT_FS_CAPABILITIES(handle, p_ts_res);
}

static SMB_STRUCT_DIR *vfs_svn_opendir(vfs_handle_struct *handle,  const char *fname, const char *mask, uint32 attr)
{
	return SMB_VFS_NEXT_OPENDIR(handle, fname, mask, attr);
}

static SMB_STRUCT_DIRENT *vfs_svn_readdir(vfs_handle_struct *handle,
				       SMB_STRUCT_DIR *dirp,
				       SMB_STRUCT_STAT *sbuf)
{
	return SMB_VFS_NEXT_READDIR(handle, dirp, sbuf);
}

static void vfs_svn_seekdir(vfs_handle_struct *handle,  SMB_STRUCT_DIR *dirp, long offset)
{
	SMB_VFS_NEXT_SEEKDIR(handle, dirp, offset);
}

static long vfs_svn_telldir(vfs_handle_struct *handle,  SMB_STRUCT_DIR *dirp)
{
	return SMB_VFS_NEXT_TELLDIR(handle, dirp);
}

static void vfs_svn_rewind_dir(vfs_handle_struct *handle, SMB_STRUCT_DIR *dirp)
{
	SMB_VFS_NEXT_REWINDDIR(handle, dirp);
}

static int vfs_svn_mkdir(vfs_handle_struct *handle,  const char *path, mode_t mode)
{
	return SMB_VFS_NEXT_MKDIR(handle, path, mode);
}

static int vfs_svn_rmdir(vfs_handle_struct *handle,  const char *path)
{
	return SMB_VFS_NEXT_RMDIR(handle, path);
}

static int vfs_svn_closedir(vfs_handle_struct *handle,  SMB_STRUCT_DIR *dir)
{
	return SMB_VFS_NEXT_CLOSEDIR(handle, dir);
}

static void vfs_svn_init_search_op(struct vfs_handle_struct *handle, SMB_STRUCT_DIR *dirp)
{
	SMB_VFS_NEXT_INIT_SEARCH_OP(handle, dirp);
}

static int vfs_svn_open(vfs_handle_struct *handle, struct smb_filename *smb_fname,
		     files_struct *fsp, int flags, mode_t mode)
{
	return SMB_VFS_NEXT_OPEN(handle, smb_fname, fsp, flags, mode);
}

/*
static NTSTATUS vfs_svn_create_file(struct vfs_handle_struct *handle,
                                struct smb_request *req,
                                uint16_t root_dir_fid,
                                struct smb_filename *smb_fname,
                                uint32_t access_mask,
                                uint32_t share_access,
                                uint32_t create_disposition,
                                uint32_t create_options,
                                struct security_descriptor *sd,
                                uint32_t file_attributes,
                                uint32_t oplock_request,
                                uint64_t allocation_size,
                                struct ea_list *ea_list,
                                files_struct **result,
                                int *pinfo)
{
	return SMB_VFS_NEXT_CREATE_FILE(handle,
				req,
				root_dir_fid,
				smb_fname,
				access_mask,
				share_access,
				create_disposition,
				create_options,
				file_attributes,
				oplock_request,
				allocation_size,
				sd,
				ea_list,
				result,
				pinfo);
}
*/

static int vfs_svn_close_fn(vfs_handle_struct *handle, files_struct *fsp)
{
/*
	openlog ("vfs_svn", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);         
	syslog (LOG_NOTICE, "vfs_svn_close_fn requested by User %d", getuid ());
	closelog ();
        */

/*
http://www.sfr-fresh.com/unix/misc/samba-3.6.0pre1.tar.gz/dox/structvfs__handle__struct.html
http://svn.collab.net/svn-doxygen/group__Info.html
lynx /usr/share/doc/libsvn1/doc
*/
	/*
        char abspath_or_url[] = '/home/alic/workspace/authorslive/pom.xml';
	svn_error_t status;

	svn_client_ctx_t ctx;
	apr_pool_t scratch_pool;

//svn_info_receiver2_t receiver;

	status = svn_client_info3 (abspath_or_url,
		svn_opt_revision_unspecified,
		svn_opt_revision_unspecified,
		receiver,
		void,
		svn_depth_empty,
		NULL,
		ctx,
		scratch_pool
	);
        */
	
/*
	svn_info_t::wcroot_abspath
*/

	
	return SMB_VFS_NEXT_CLOSE(handle, fsp);
}

static ssize_t vfs_svn_vfs_read(vfs_handle_struct *handle, files_struct *fsp, void *data, size_t n)
{
	return SMB_VFS_NEXT_READ(handle, fsp, data, n);
}

static ssize_t vfs_svn_pread(vfs_handle_struct *handle, files_struct *fsp, void *data, size_t n, SMB_OFF_T offset)
{
	return SMB_VFS_NEXT_PREAD(handle, fsp, data, n, offset);
}

static ssize_t vfs_svn_write(vfs_handle_struct *handle, files_struct *fsp, const void *data, size_t n)
{
	return SMB_VFS_NEXT_WRITE(handle, fsp, data, n);
}

static ssize_t vfs_svn_pwrite(vfs_handle_struct *handle, files_struct *fsp, const void *data, size_t n, SMB_OFF_T offset)
{
	return SMB_VFS_NEXT_PWRITE(handle, fsp, data, n, offset);
}

static SMB_OFF_T vfs_svn_lseek(vfs_handle_struct *handle, files_struct *fsp, SMB_OFF_T offset, int whence)
{
	return SMB_VFS_NEXT_LSEEK(handle, fsp, offset, whence);
}

static ssize_t vfs_svn_sendfile(vfs_handle_struct *handle, int tofd, files_struct *fromfsp, const DATA_BLOB *hdr, SMB_OFF_T offset, size_t n)
{
	return SMB_VFS_NEXT_SENDFILE(handle, tofd, fromfsp, hdr, offset, n);
}

static ssize_t vfs_svn_recvfile(vfs_handle_struct *handle, int fromfd, files_struct *tofsp, SMB_OFF_T offset, size_t n)
{
	return SMB_VFS_NEXT_RECVFILE(handle, fromfd, tofsp, offset, n);
}

static int vfs_svn_rename(vfs_handle_struct *handle,
		       const struct smb_filename *smb_fname_src,
		       const struct smb_filename *smb_fname_dst)
{
	return SMB_VFS_NEXT_RENAME(handle, smb_fname_src, smb_fname_dst);
}

static int vfs_svn_fsync(vfs_handle_struct *handle, files_struct *fsp)
{
	return SMB_VFS_NEXT_FSYNC(handle, fsp);
}

static int vfs_svn_stat(vfs_handle_struct *handle, struct smb_filename *smb_fname)
{
	return SMB_VFS_NEXT_STAT(handle, smb_fname);
}

static int vfs_svn_fstat(vfs_handle_struct *handle, files_struct *fsp, SMB_STRUCT_STAT *sbuf)
{
	return SMB_VFS_NEXT_FSTAT(handle, fsp, sbuf);
}

static int vfs_svn_lstat(vfs_handle_struct *handle, struct smb_filename *smb_fname)
{
	return SMB_VFS_NEXT_LSTAT(handle, smb_fname);
}

static uint64_t vfs_svn_get_alloc_size(struct vfs_handle_struct *handle, struct files_struct *fsp, const SMB_STRUCT_STAT *sbuf)
{
	return SMB_VFS_NEXT_GET_ALLOC_SIZE(handle, fsp, sbuf);
}

static int vfs_svn_unlink(vfs_handle_struct *handle,
		       const struct smb_filename *smb_fname)
{
	return SMB_VFS_NEXT_UNLINK(handle, smb_fname);
}

static int vfs_svn_chmod(vfs_handle_struct *handle,  const char *path, mode_t mode)
{
	return SMB_VFS_NEXT_CHMOD(handle, path, mode);
}

static int vfs_svn_fchmod(vfs_handle_struct *handle, files_struct *fsp, mode_t mode)
{
	return SMB_VFS_NEXT_FCHMOD(handle, fsp, mode);
}

static int vfs_svn_chown(vfs_handle_struct *handle,  const char *path, uid_t uid, gid_t gid)
{
	return SMB_VFS_NEXT_CHOWN(handle, path, uid, gid);
}

static int vfs_svn_fchown(vfs_handle_struct *handle, files_struct *fsp, uid_t uid, gid_t gid)
{
	return SMB_VFS_NEXT_FCHOWN(handle, fsp, uid, gid);
}

static int vfs_svn_lchown(vfs_handle_struct *handle,  const char *path, uid_t uid, gid_t gid)
{
	return SMB_VFS_NEXT_LCHOWN(handle, path, uid, gid);
}

static int vfs_svn_chdir(vfs_handle_struct *handle,  const char *path)
{
	return SMB_VFS_NEXT_CHDIR(handle, path);
}

static char *vfs_svn_getwd(vfs_handle_struct *handle,  char *buf)
{
	return SMB_VFS_NEXT_GETWD(handle, buf);
}

static int vfs_svn_ntimes(vfs_handle_struct *handle,
		       const struct smb_filename *smb_fname,
		       struct smb_file_time *ft)
{
	return SMB_VFS_NEXT_NTIMES(handle, smb_fname, ft);
}

static int vfs_svn_ftruncate(vfs_handle_struct *handle, files_struct *fsp, SMB_OFF_T offset)
{
	return SMB_VFS_NEXT_FTRUNCATE(handle, fsp, offset);
}

static bool vfs_svn_lock(vfs_handle_struct *handle, files_struct *fsp, int op, SMB_OFF_T offset, SMB_OFF_T count, int type)
{
	return SMB_VFS_NEXT_LOCK(handle, fsp, op, offset, count, type);
}

static int vfs_svn_kernel_flock(struct vfs_handle_struct *handle, struct files_struct *fsp, uint32 share_mode, uint32 access_mask)
{
	return SMB_VFS_NEXT_KERNEL_FLOCK(handle, fsp, share_mode, access_mask);
}

static int vfs_svn_linux_setlease(struct vfs_handle_struct *handle, struct files_struct *fsp, int leasetype)
{
	return SMB_VFS_NEXT_LINUX_SETLEASE(handle, fsp, leasetype);
}

static bool vfs_svn_getlock(vfs_handle_struct *handle, files_struct *fsp, SMB_OFF_T *poffset, SMB_OFF_T *pcount, int *ptype, pid_t *ppid)
{
	return SMB_VFS_NEXT_GETLOCK(handle, fsp, poffset, pcount, ptype, ppid);
}

static int vfs_svn_symlink(vfs_handle_struct *handle,  const char *oldpath, const char *newpath)
{
	return SMB_VFS_NEXT_SYMLINK(handle, oldpath, newpath);
}

static int vfs_svn_vfs_readlink(vfs_handle_struct *handle, const char *path, char *buf, size_t bufsiz)
{
	return SMB_VFS_NEXT_READLINK(handle, path, buf, bufsiz);
}

static int vfs_svn_link(vfs_handle_struct *handle,  const char *oldpath, const char *newpath)
{
	return SMB_VFS_NEXT_LINK(handle, oldpath, newpath);
}

static int vfs_svn_mknod(vfs_handle_struct *handle,  const char *path, mode_t mode, SMB_DEV_T dev)
{
	return SMB_VFS_NEXT_MKNOD(handle, path, mode, dev);
}

static char *vfs_svn_realpath(vfs_handle_struct *handle,  const char *path, char *resolved_path)
{
	return SMB_VFS_NEXT_REALPATH(handle, path, resolved_path);
}

static NTSTATUS vfs_svn_notify_watch(struct vfs_handle_struct *handle,
	    struct sys_notify_context *ctx, struct notify_entry *e,
	    void (*callback)(struct sys_notify_context *ctx, void *private_data, struct notify_event *ev),
	    void *private_data, void *handle_p)
{
	return SMB_VFS_NEXT_NOTIFY_WATCH(handle, ctx, e, callback,
		private_data, handle_p);
}

static int vfs_svn_chflags(vfs_handle_struct *handle,  const char *path, uint flags)
{
	return SMB_VFS_NEXT_CHFLAGS(handle, path, flags);
}

static struct file_id vfs_svn_file_id_create(vfs_handle_struct *handle,
					  const SMB_STRUCT_STAT *sbuf)
{
	return SMB_VFS_NEXT_FILE_ID_CREATE(handle, sbuf);
}

static NTSTATUS vfs_svn_streaminfo(struct vfs_handle_struct *handle,
				struct files_struct *fsp,
				const char *fname,
				TALLOC_CTX *mem_ctx,
				unsigned int *num_streams,
				struct stream_struct **streams)
{
	return SMB_VFS_NEXT_STREAMINFO(handle,
					fsp,
					fname,
					mem_ctx,
					num_streams,
					streams);
}

static int vfs_svn_get_real_filename(struct vfs_handle_struct *handle,
				const char *path,
				const char *name,
				TALLOC_CTX *mem_ctx,
				char **found_name)
{
	return SMB_VFS_NEXT_GET_REAL_FILENAME(handle,
					path,
					name,
					mem_ctx,
					found_name);
}

static const char *vfs_svn_connectpath(struct vfs_handle_struct *handle,
				const char *filename)
{
	return SMB_VFS_NEXT_CONNECTPATH(handle, filename);
}

static NTSTATUS vfs_svn_brl_lock_windows(struct vfs_handle_struct *handle,
				struct byte_range_lock *br_lck,
				struct lock_struct *plock,
				bool blocking_lock,
				struct blocking_lock_record *blr)
{
	return SMB_VFS_NEXT_BRL_LOCK_WINDOWS(handle,
					br_lck,
					plock,
					blocking_lock,
					blr);
}

static bool vfs_svn_brl_unlock_windows(struct vfs_handle_struct *handle,
				struct messaging_context *msg_ctx,
				struct byte_range_lock *br_lck,
				const struct lock_struct *plock)
{
	return SMB_VFS_NEXT_BRL_UNLOCK_WINDOWS(handle,
					msg_ctx,
					br_lck,
					plock);
}

static bool vfs_svn_brl_cancel_windows(struct vfs_handle_struct *handle,
				struct byte_range_lock *br_lck,
				struct lock_struct *plock,
				struct blocking_lock_record *blr)
{
	return SMB_VFS_NEXT_BRL_CANCEL_WINDOWS(handle,
				br_lck,
				plock,
				blr);
}

static bool vfs_svn_strict_lock(struct vfs_handle_struct *handle,
				struct files_struct *fsp,
				struct lock_struct *plock)
{
	return SMB_VFS_NEXT_STRICT_LOCK(handle,
					fsp,
					plock);
}

static void vfs_svn_strict_unlock(struct vfs_handle_struct *handle,
				struct files_struct *fsp,
				struct lock_struct *plock)
{
	SMB_VFS_NEXT_STRICT_UNLOCK(handle,
					fsp,
					plock);
}

static NTSTATUS vfs_svn_translate_name(struct vfs_handle_struct *handle,
				const char *mapped_name,
				enum vfs_translate_direction direction,
				TALLOC_CTX *mem_ctx,
				char **pmapped_name)
{
	return SMB_VFS_NEXT_TRANSLATE_NAME(handle, mapped_name, direction,
					   mem_ctx, pmapped_name);
}

static NTSTATUS vfs_svn_fget_nt_acl(vfs_handle_struct *handle, files_struct *fsp,
	uint32 security_info, SEC_DESC **ppdesc)
{
	return SMB_VFS_NEXT_FGET_NT_ACL(handle, fsp, security_info, ppdesc);
}

static NTSTATUS vfs_svn_get_nt_acl(vfs_handle_struct *handle,
	const char *name, uint32 security_info, SEC_DESC **ppdesc)
{
	return SMB_VFS_NEXT_GET_NT_ACL(handle, name, security_info, ppdesc);
}

static NTSTATUS vfs_svn_fset_nt_acl(vfs_handle_struct *handle, files_struct *fsp,
	uint32 security_info_sent, const SEC_DESC *psd)
{
	return SMB_VFS_NEXT_FSET_NT_ACL(handle, fsp, security_info_sent, psd);
}

static int vfs_svn_chmod_acl(vfs_handle_struct *handle,  const char *name, mode_t mode)
{
	return SMB_VFS_NEXT_CHMOD_ACL(handle, name, mode);
}

static int vfs_svn_fchmod_acl(vfs_handle_struct *handle, files_struct *fsp, mode_t mode)
{
	return SMB_VFS_NEXT_FCHMOD_ACL(handle, fsp, mode);
}

static int vfs_svn_sys_acl_get_entry(vfs_handle_struct *handle,  SMB_ACL_T theacl, int entry_id, SMB_ACL_ENTRY_T *entry_p)
{
	return SMB_VFS_NEXT_SYS_ACL_GET_ENTRY(handle, theacl, entry_id, entry_p);
}

static int vfs_svn_sys_acl_get_tag_type(vfs_handle_struct *handle,  SMB_ACL_ENTRY_T entry_d, SMB_ACL_TAG_T *tag_type_p)
{
	return SMB_VFS_NEXT_SYS_ACL_GET_TAG_TYPE(handle, entry_d, tag_type_p);
}

static int vfs_svn_sys_acl_get_permset(vfs_handle_struct *handle,  SMB_ACL_ENTRY_T entry_d, SMB_ACL_PERMSET_T *permset_p)
{
	return SMB_VFS_NEXT_SYS_ACL_GET_PERMSET(handle, entry_d, permset_p);
}

static void *vfs_svn_sys_acl_get_qualifier(vfs_handle_struct *handle,  SMB_ACL_ENTRY_T entry_d)
{
	return SMB_VFS_NEXT_SYS_ACL_GET_QUALIFIER(handle, entry_d);
}

static SMB_ACL_T vfs_svn_sys_acl_get_file(vfs_handle_struct *handle,  const char *path_p, SMB_ACL_TYPE_T type)
{
	return SMB_VFS_NEXT_SYS_ACL_GET_FILE(handle, path_p, type);
}

static SMB_ACL_T vfs_svn_sys_acl_get_fd(vfs_handle_struct *handle, files_struct *fsp)
{
	return SMB_VFS_NEXT_SYS_ACL_GET_FD(handle, fsp);
}

static int vfs_svn_sys_acl_clear_perms(vfs_handle_struct *handle,  SMB_ACL_PERMSET_T permset)
{
	return SMB_VFS_NEXT_SYS_ACL_CLEAR_PERMS(handle, permset);
}

static int vfs_svn_sys_acl_add_perm(vfs_handle_struct *handle,  SMB_ACL_PERMSET_T permset, SMB_ACL_PERM_T perm)
{
	return SMB_VFS_NEXT_SYS_ACL_ADD_PERM(handle, permset, perm);
}

static char *vfs_svn_sys_acl_to_text(vfs_handle_struct *handle,  SMB_ACL_T theacl, ssize_t *plen)
{
	return SMB_VFS_NEXT_SYS_ACL_TO_TEXT(handle, theacl, plen);
}

static SMB_ACL_T vfs_svn_sys_acl_init(vfs_handle_struct *handle,  int count)
{
	return SMB_VFS_NEXT_SYS_ACL_INIT(handle, count);
}

static int vfs_svn_sys_acl_create_entry(vfs_handle_struct *handle,  SMB_ACL_T *pacl, SMB_ACL_ENTRY_T *pentry)
{
	return SMB_VFS_NEXT_SYS_ACL_CREATE_ENTRY(handle, pacl, pentry);
}

static int vfs_svn_sys_acl_set_tag_type(vfs_handle_struct *handle,  SMB_ACL_ENTRY_T entry, SMB_ACL_TAG_T tagtype)
{
	return SMB_VFS_NEXT_SYS_ACL_SET_TAG_TYPE(handle, entry, tagtype);
}

static int vfs_svn_sys_acl_set_qualifier(vfs_handle_struct *handle,  SMB_ACL_ENTRY_T entry, void *qual)
{
	return SMB_VFS_NEXT_SYS_ACL_SET_QUALIFIER(handle, entry, qual);
}

static int vfs_svn_sys_acl_set_permset(vfs_handle_struct *handle,  SMB_ACL_ENTRY_T entry, SMB_ACL_PERMSET_T permset)
{
	return SMB_VFS_NEXT_SYS_ACL_SET_PERMSET(handle, entry, permset);
}

static int vfs_svn_sys_acl_valid(vfs_handle_struct *handle,  SMB_ACL_T theacl )
{
	return SMB_VFS_NEXT_SYS_ACL_VALID(handle, theacl);
}

static int vfs_svn_sys_acl_set_file(vfs_handle_struct *handle,  const char *name, SMB_ACL_TYPE_T acltype, SMB_ACL_T theacl)
{
	return SMB_VFS_NEXT_SYS_ACL_SET_FILE(handle, name, acltype, theacl);
}

static int vfs_svn_sys_acl_set_fd(vfs_handle_struct *handle, files_struct *fsp, SMB_ACL_T theacl)
{
	return SMB_VFS_NEXT_SYS_ACL_SET_FD(handle, fsp, theacl);
}

static int vfs_svn_sys_acl_delete_def_file(vfs_handle_struct *handle,  const char *path)
{
	return SMB_VFS_NEXT_SYS_ACL_DELETE_DEF_FILE(handle, path);
}

static int vfs_svn_sys_acl_get_perm(vfs_handle_struct *handle,  SMB_ACL_PERMSET_T permset, SMB_ACL_PERM_T perm)
{
	return SMB_VFS_NEXT_SYS_ACL_GET_PERM(handle, permset, perm);
}

static int vfs_svn_sys_acl_free_text(vfs_handle_struct *handle,  char *text)
{
	return SMB_VFS_NEXT_SYS_ACL_FREE_TEXT(handle, text);
}

static int vfs_svn_sys_acl_free_acl(vfs_handle_struct *handle,  SMB_ACL_T posix_acl)
{
	return SMB_VFS_NEXT_SYS_ACL_FREE_ACL(handle, posix_acl);
}

static int vfs_svn_sys_acl_free_qualifier(vfs_handle_struct *handle,  void *qualifier, SMB_ACL_TAG_T tagtype)
{
	return SMB_VFS_NEXT_SYS_ACL_FREE_QUALIFIER(handle, qualifier, tagtype);
}

static ssize_t vfs_svn_getxattr(vfs_handle_struct *handle, const char *path, const char *name, void *value, size_t size)
{
        return SMB_VFS_NEXT_GETXATTR(handle, path, name, value, size);
}

static ssize_t vfs_svn_lgetxattr(vfs_handle_struct *handle, const char *path, const char *name, void *value, size_t
size)
{
        return SMB_VFS_NEXT_LGETXATTR(handle, path, name, value, size);
}

static ssize_t vfs_svn_fgetxattr(vfs_handle_struct *handle, struct files_struct *fsp, const char *name, void *value, size_t size)
{
        return SMB_VFS_NEXT_FGETXATTR(handle, fsp, name, value, size);
}

static ssize_t vfs_svn_listxattr(vfs_handle_struct *handle, const char *path, char *list, size_t size)
{
        return SMB_VFS_NEXT_LISTXATTR(handle, path, list, size);
}

static ssize_t vfs_svn_llistxattr(vfs_handle_struct *handle, const char *path, char *list, size_t size)
{
        return SMB_VFS_NEXT_LLISTXATTR(handle, path, list, size);
}

static ssize_t vfs_svn_flistxattr(vfs_handle_struct *handle, struct files_struct *fsp, char *list, size_t size)
{
        return SMB_VFS_NEXT_FLISTXATTR(handle, fsp, list, size);
}

static int vfs_svn_removexattr(vfs_handle_struct *handle, const char *path, const char *name)
{
        return SMB_VFS_NEXT_REMOVEXATTR(handle, path, name);
}

static int vfs_svn_lremovexattr(vfs_handle_struct *handle, const char *path, const char *name)
{
        return SMB_VFS_NEXT_LREMOVEXATTR(handle, path, name);
}

static int vfs_svn_fremovexattr(vfs_handle_struct *handle, struct files_struct *fsp, const char *name)
{
        return SMB_VFS_NEXT_FREMOVEXATTR(handle, fsp, name);
}

static int vfs_svn_setxattr(vfs_handle_struct *handle, const char *path, const char *name, const void *value, size_t size, int flags)
{
        return SMB_VFS_NEXT_SETXATTR(handle, path, name, value, size, flags);
}

static int vfs_svn_lsetxattr(vfs_handle_struct *handle, const char *path, const char *name, const void *value, size_t size, int flags)
{
        return SMB_VFS_NEXT_LSETXATTR(handle, path, name, value, size, flags);
}

static int vfs_svn_fsetxattr(vfs_handle_struct *handle, struct files_struct *fsp, const char *name, const void *value, size_t size, int flags)
{
        return SMB_VFS_NEXT_FSETXATTR(handle, fsp, name, value, size, flags);
}

static int vfs_svn_aio_read(struct vfs_handle_struct *handle, struct files_struct *fsp, SMB_STRUCT_AIOCB *aiocb)
{
	return SMB_VFS_NEXT_AIO_READ(handle, fsp, aiocb);
}

static int vfs_svn_aio_write(struct vfs_handle_struct *handle, struct files_struct *fsp, SMB_STRUCT_AIOCB *aiocb)
{
	return SMB_VFS_NEXT_AIO_WRITE(handle, fsp, aiocb);
}

static ssize_t vfs_svn_aio_return_fn(struct vfs_handle_struct *handle, struct files_struct *fsp, SMB_STRUCT_AIOCB *aiocb)
{
	return SMB_VFS_NEXT_AIO_RETURN(handle, fsp, aiocb);
}

static int vfs_svn_aio_cancel(struct vfs_handle_struct *handle, struct files_struct *fsp, SMB_STRUCT_AIOCB *aiocb)
{
	return SMB_VFS_NEXT_AIO_CANCEL(handle, fsp, aiocb);
}

static int vfs_svn_aio_error_fn(struct vfs_handle_struct *handle, struct files_struct *fsp, SMB_STRUCT_AIOCB *aiocb)
{
	return SMB_VFS_NEXT_AIO_ERROR(handle, fsp, aiocb);
}

static int vfs_svn_aio_fsync(struct vfs_handle_struct *handle, struct files_struct *fsp, int op, SMB_STRUCT_AIOCB *aiocb)
{
	return SMB_VFS_NEXT_AIO_FSYNC(handle, fsp, op, aiocb);
}

static int vfs_svn_aio_suspend(struct vfs_handle_struct *handle, struct files_struct *fsp, const SMB_STRUCT_AIOCB * const aiocb[], int n, const struct timespec *ts)
{
	return SMB_VFS_NEXT_AIO_SUSPEND(handle, fsp, aiocb, n, ts);
}

static bool vfs_svn_aio_force(struct vfs_handle_struct *handle, struct files_struct *fsp)
{
        return SMB_VFS_NEXT_AIO_FORCE(handle, fsp);
}

static bool vfs_svn_is_offline(struct vfs_handle_struct *handle, const char *path, SMB_STRUCT_STAT *sbuf)
{
	return SMB_VFS_NEXT_IS_OFFLINE(handle, path, sbuf);
}

static int vfs_svn_set_offline(struct vfs_handle_struct *handle, const char *path)
{
	return SMB_VFS_NEXT_SET_OFFLINE(handle, path);
}

/* VFS operations structure */

struct vfs_fn_pointers vfs_svn_fns = {
	/* Disk operations */

	.connect_fn = vfs_svn_connect,
	.disconnect = vfs_svn_disconnect,
	.disk_free = vfs_svn_disk_free,
	.get_quota = vfs_svn_get_quota,
	.set_quota = vfs_svn_set_quota,
	.get_shadow_copy_data = vfs_svn_get_shadow_copy_data,
	.statvfs = vfs_svn_statvfs,
	.fs_capabilities = vfs_svn_fs_capabilities,

	/* Directory operations */

	.opendir = vfs_svn_opendir,
	.readdir = vfs_svn_readdir,
	.seekdir = vfs_svn_seekdir,
	.telldir = vfs_svn_telldir,
	.rewind_dir = vfs_svn_rewind_dir,
	.mkdir = vfs_svn_mkdir,
	.rmdir = vfs_svn_rmdir,
	.closedir = vfs_svn_closedir,
	.init_search_op = vfs_svn_init_search_op,

	/* File operations */

	.open = vfs_svn_open,
//	.create_file = vfs_svn_create_file,
	.close_fn = vfs_svn_close_fn,
	.vfs_read = vfs_svn_vfs_read,
	.pread = vfs_svn_pread,
	.write = vfs_svn_write,
	.pwrite = vfs_svn_pwrite,
	.lseek = vfs_svn_lseek,
	.sendfile = vfs_svn_sendfile,
	.recvfile = vfs_svn_recvfile,
	.rename = vfs_svn_rename,
	.fsync = vfs_svn_fsync,
	.stat = vfs_svn_stat,
	.fstat = vfs_svn_fstat,
	.lstat = vfs_svn_lstat,
	.get_alloc_size = vfs_svn_get_alloc_size,
	.unlink = vfs_svn_unlink,
	.chmod = vfs_svn_chmod,
	.fchmod = vfs_svn_fchmod,
	.chown = vfs_svn_chown,
	.fchown = vfs_svn_fchown,
	.lchown = vfs_svn_lchown,
	.chdir = vfs_svn_chdir,
	.getwd = vfs_svn_getwd,
	.ntimes = vfs_svn_ntimes,
	.ftruncate = vfs_svn_ftruncate,
	.lock = vfs_svn_lock,
	.kernel_flock = vfs_svn_kernel_flock,
	.linux_setlease = vfs_svn_linux_setlease,
	.getlock = vfs_svn_getlock,
	.symlink = vfs_svn_symlink,
	.vfs_readlink = vfs_svn_vfs_readlink,
	.link = vfs_svn_link,
	.mknod = vfs_svn_mknod,
	.realpath = vfs_svn_realpath,
	.notify_watch = vfs_svn_notify_watch,
	.chflags = vfs_svn_chflags,
	.file_id_create = vfs_svn_file_id_create,

	.streaminfo = vfs_svn_streaminfo,
	.get_real_filename = vfs_svn_get_real_filename,
	.connectpath = vfs_svn_connectpath,
	.brl_lock_windows = vfs_svn_brl_lock_windows,
	.brl_unlock_windows = vfs_svn_brl_unlock_windows,
	.brl_cancel_windows = vfs_svn_brl_cancel_windows,
	.strict_lock = vfs_svn_strict_lock,
	.strict_unlock = vfs_svn_strict_unlock,
	.translate_name = vfs_svn_translate_name,

	/* NT ACL operations. */

	.fget_nt_acl = vfs_svn_fget_nt_acl,
	.get_nt_acl = vfs_svn_get_nt_acl,
	.fset_nt_acl = vfs_svn_fset_nt_acl,

	/* POSIX ACL operations. */

	.chmod_acl = vfs_svn_chmod_acl,
	.fchmod_acl = vfs_svn_fchmod_acl,

	.sys_acl_get_entry = vfs_svn_sys_acl_get_entry,
	.sys_acl_get_tag_type = vfs_svn_sys_acl_get_tag_type,
	.sys_acl_get_permset = vfs_svn_sys_acl_get_permset,
	.sys_acl_get_qualifier = vfs_svn_sys_acl_get_qualifier,
	.sys_acl_get_file = vfs_svn_sys_acl_get_file,
	.sys_acl_get_fd = vfs_svn_sys_acl_get_fd,
	.sys_acl_clear_perms = vfs_svn_sys_acl_clear_perms,
	.sys_acl_add_perm = vfs_svn_sys_acl_add_perm,
	.sys_acl_to_text = vfs_svn_sys_acl_to_text,
	.sys_acl_init = vfs_svn_sys_acl_init,
	.sys_acl_create_entry = vfs_svn_sys_acl_create_entry,
	.sys_acl_set_tag_type = vfs_svn_sys_acl_set_tag_type,
	.sys_acl_set_qualifier = vfs_svn_sys_acl_set_qualifier,
	.sys_acl_set_permset = vfs_svn_sys_acl_set_permset,
	.sys_acl_valid = vfs_svn_sys_acl_valid,
	.sys_acl_set_file = vfs_svn_sys_acl_set_file,
	.sys_acl_set_fd = vfs_svn_sys_acl_set_fd,
	.sys_acl_delete_def_file = vfs_svn_sys_acl_delete_def_file,
	.sys_acl_get_perm = vfs_svn_sys_acl_get_perm,
	.sys_acl_free_text = vfs_svn_sys_acl_free_text,
	.sys_acl_free_acl = vfs_svn_sys_acl_free_acl,
	.sys_acl_free_qualifier = vfs_svn_sys_acl_free_qualifier,

	/* EA operations. */
	.getxattr = vfs_svn_getxattr,
	.lgetxattr = vfs_svn_lgetxattr,
	.fgetxattr = vfs_svn_fgetxattr,
	.listxattr = vfs_svn_listxattr,
	.llistxattr = vfs_svn_llistxattr,
	.flistxattr = vfs_svn_flistxattr,
	.removexattr = vfs_svn_removexattr,
	.lremovexattr = vfs_svn_lremovexattr,
	.fremovexattr = vfs_svn_fremovexattr,
	.setxattr = vfs_svn_setxattr,
	.lsetxattr = vfs_svn_lsetxattr,
	.fsetxattr = vfs_svn_fsetxattr,

	/* aio operations */
	.aio_read = vfs_svn_aio_read,
	.aio_write = vfs_svn_aio_write,
	.aio_return_fn = vfs_svn_aio_return_fn,
	.aio_cancel = vfs_svn_aio_cancel,
	.aio_error_fn = vfs_svn_aio_error_fn,
	.aio_fsync = vfs_svn_aio_fsync,
	.aio_suspend = vfs_svn_aio_suspend,
	.aio_force = vfs_svn_aio_force,

	/* offline operations */
	.is_offline = vfs_svn_is_offline,
	.set_offline = vfs_svn_set_offline
};

NTSTATUS vfs_svn_init(void);
NTSTATUS vfs_svn_init(void)
{
	NTSTATUS ret = smb_register_vfs(SMB_VFS_INTERFACE_VERSION, "samba-vfs-subversion", &vfs_svn_fns);

        if (!NT_STATUS_IS_OK(ret))
                return ret;

        // do local init stuff here
        return ret;
}
