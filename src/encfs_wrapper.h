#ifndef _ENCFS_WRAPPER_H
#define _ENCFS_WRAPPER_H

// returns non-zero on error, in which case output will
// be an error message.
//
// *output must be freed by the caller.
int encfs_stash_get_info (const char *crypt_dir, char **output);
int encfs_stash_change_password (const char *crypt_dir, const char *old_password, const char *new_password, char **output);
int encfs_stash_new (const char *crypt_dir, const char *mount_dir, const char *password);
int encfs_stash_mount(const char *crypt_dir, const char *mount_dir, const char *password, int idle_timeout, char **output);
int encfs_stash_unmount (const char *mount_dir);
bool is_mounted(const char *mount_dir);

#endif /* _ENCFS_WRAPPER_H */
