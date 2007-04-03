#ifndef _CRYPTKEEPER_H
#define _CRYPTKEEPER_H

bool make_new_encfs_stash (const char *mount_dir, const char *password);
void add_crypt_point (const char *stash_dir, const char *mount_dir);
void write_config ();

extern char *config_filemanager;
extern int config_idletime;

#endif /* _CRYPTKEEPER_H */
