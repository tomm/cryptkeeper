#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

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

	int pid = fork ();

	mkdir (crypt_dir, 0700);
	mkdir (mount_dir, 0700);

	if (pid == 0) {
		dup2 (fd[0], 0);
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
	return status;
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
	rmdir (mount_dir);
	return status;
}
