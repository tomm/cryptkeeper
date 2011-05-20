/*
 * This file is part of Cryptkeeper.
 * Copyright (C) 2007 Tom Morton
 *
 * Cryptkeeper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * Cryptkeeper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mntent.h>
#include <poll.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

// only for the gettext _
#include "cryptkeeper.h"

bool is_mounted(const char *mount_dir)
{
	struct mntent *m;
	FILE *f = setmntent("/etc/mtab", "r");
 	char *mount_dir_expanded, *mnt_dir_expanded;

	if (!(mount_dir_expanded = realpath(mount_dir, NULL))) {
		// no such file or dir, ...
		// so: not mounted
		//	  perror("cryptkeeper, is_mounted");
		return false;
	}
	while(m = getmntent(f)) {
 	        if(mnt_dir_expanded = realpath(m->mnt_dir, NULL)) {
			if (strcmp(mount_dir_expanded, mnt_dir_expanded)==0) {
				free(mnt_dir_expanded);
				free(mount_dir_expanded);
				return true;
			}
			free(mnt_dir_expanded);
		}
	}
	free(mount_dir_expanded);
	return false;
}

int encfs_stash_get_info (const char *crypt_dir, char **output)
{
	int status_fd[2];

	*output = NULL;

	assert (pipe (status_fd) == 0);

	int pid = fork ();

	if (pid == 0) {
		dup2 (status_fd[1], 1);
		close (status_fd[0]);
		execlp ("encfsctl", "encfsctl", "info", crypt_dir, NULL);
		exit (0);
	}
	close (status_fd[1]);

	char buf[1024];
	memset (buf, 0, sizeof (buf));
	read (status_fd[0], buf, sizeof (buf));
	
	int status;
	waitpid (pid, &status, 0);
	*output = strdup (buf);
	return status;
}

int encfs_stash_change_password (const char *crypt_dir, const char *old_password, const char *new_password, char **output)
{
	int fd[2], status_fd[2];

	assert (pipe (fd) == 0);
	assert (pipe (status_fd) == 0);

	int pid = fork ();

	if (pid == 0) {
		dup2 (fd[0], 0);
		dup2 (status_fd[1], 1);
		close (fd[1]);
		close (status_fd[0]);
		execlp ("encfsctl", "encfsctl", "autopasswd", crypt_dir, NULL);
		exit (0);
	}
	close (fd[0]);
	close (status_fd[1]);

	write (fd[1], old_password, strlen (old_password));
	write (fd[1], "\n", 1);
	write (fd[1], new_password, strlen (new_password));
	write (fd[1], "\n", 1);
	close (fd[1]);

	char buf[1024];
	memset (buf, 0, sizeof (buf));
	read (status_fd[0], buf, sizeof (buf));
	//printf ("YES [%s]\n", buf);
	char *wank;
	wank = strchr (buf, '\n');
	//if (wank) wank = strchr (wank+1, '\n');
	//printf ("YEES [%s]\n", wank);
	close (status_fd[0]);

	int status;
	waitpid (pid, &status, 0);
	*output = strdup (wank);
	return status;
}

int encfs_stash_new (const char *crypt_dir, const char *mount_dir, const char *password)
{
	int fd[2];

	assert (pipe (fd) == 0);

	mkdir (crypt_dir, 0700);
	mkdir (mount_dir, 0700);

	int pid = fork ();

	if (pid == 0) {
		dup2 (fd[0], 0);
		// don't want to see encfses stdout bullshit
		int devnull = open("/dev/null", O_WRONLY);
		dup2(devnull, 1);
		close (fd[1]);
		execlp ("encfs", "encfs", "-S", crypt_dir, mount_dir, NULL);
		exit (0);
	}
	close (fd[0]);
	// paranoid default setup mode
	//write (fd[1], "y\n", 2);
	//write (fd[1], "y\n", 2);
	write (fd[1], "p\n", 2);
	write (fd[1], password, strlen (password));
	write (fd[1], "\n", 1);
	close (fd[1]);
	int status;
	waitpid (pid, &status, 0);
	return !is_mounted(mount_dir);
//	return status;
}

int encfs_stash_mount(const char *crypt_dir, const char *mount_dir, const char *password, int idle_timeout, 
                      bool allow_other, char **output)
{
	mkdir (mount_dir, 0700);
	if (rmdir (mount_dir) == -1) {
		char buf[256];
		snprintf(buf, sizeof(buf), "%s\n\n%s",
				_("The encrypted folder could not be mounted because the mount point is not empty:"),
				mount_dir);
		*output = strdup(buf);
		return 255;
	}
	mkdir (mount_dir, 0700);

	int fd[2], status_fd[2];

	assert(pipe(fd) == 0);
	assert(pipe(status_fd) == 0);

	int pid = fork ();
	if (pid == 0) {
		dup2(fd[0], 0);
		dup2(status_fd[1], 1);
		dup2(status_fd[1], 2);
		close(fd[1]);
		close(status_fd[0]);
		if (idle_timeout == 0) {
			if (allow_other) {
				execlp ("encfs", "encfs", "-S", crypt_dir, mount_dir, "--", "-o", "allow_other", NULL);
			} else {
				execlp ("encfs", "encfs", "-S", crypt_dir, mount_dir, NULL);
			}
		} else {
			char buf[256];
			snprintf (buf, sizeof (buf), "--idle=%d", idle_timeout);
			if (allow_other) {
				execlp ("encfs", "encfs", buf, "-S", crypt_dir, mount_dir, "--", "-o", "allow_other", NULL);
			}
			else {
				execlp ("encfs", "encfs", buf, "-S", crypt_dir, mount_dir, NULL);
			}
		}
		exit (0);
	}
	close(status_fd[1]);
	close(fd[0]);
	write(fd[1], password, strlen(password));
	write(fd[1], "\n", 1);
	close(fd[1]);

	// unfortunately encfs does not report errors in a consistent way. eg
	// the 'mountpoint is not empty' error is not written to fd=1 but the
	// password incorrect error msg is -- hence the stupid crap at the
	// start of this function
	struct pollfd pfd;
	pfd.fd = status_fd[0];
	pfd.events = POLLIN;
	poll(&pfd, 1, 1000);
	
	if (pfd.revents & POLLIN) {
		// encfs has a message for us 
		char buf[256];
		memset(buf,0,sizeof(buf));
		read(status_fd[0], buf, sizeof(buf));
		close(status_fd[0]);
		*output = strdup(buf);
	} else {
		*output = NULL;
	}

	int status;
	waitpid (pid, &status, 0);
	//printf("status %d\n", status);
	//return status;
	//return output != NULL
	// encfs returned status is useless, so we just have to test like this...
	return !is_mounted(mount_dir);
}

int encfs_stash_unmount (const char *mount_dir)
{
	// unmount
	int pid = fork ();
	if (pid == 0) {
		execlp ("fusermount", "fusermount", "-u", mount_dir, NULL);
		exit (0);
	}
	int status;
	waitpid (pid, &status, 0);
	//return status;
	return is_mounted(mount_dir);
}
