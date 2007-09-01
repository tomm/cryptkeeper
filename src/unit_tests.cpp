#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "encfs_wrapper.h"

#define PASSWORD1	"hello old bean"
#define PASSWORD2	"the new password"

static int passed, failed;

#define TEST(msg,x)	{ if(0==(x)) {printf("PASSED ");passed++;} else {printf("FAILED ");failed++;} puts(msg); }
#define OUTPUT()	{ printf("	output was: '%s'\n", output); if (output) free(output); output=NULL; }

int main()
{
	char *output;
	char crypt_dir[256], mount_dir[256];
	snprintf(crypt_dir, sizeof(crypt_dir), "%s/unit_test_crypt", getenv("PWD"));
	snprintf(mount_dir, sizeof(mount_dir), "%s/unit_test_mount", getenv("PWD"));

	TEST("create", encfs_stash_new(crypt_dir, mount_dir, PASSWORD1));
	TEST("unmount", encfs_stash_unmount(mount_dir));
	TEST("mount", encfs_stash_mount(crypt_dir, mount_dir, PASSWORD1, 0, &output));
	OUTPUT();
	TEST("unmount", encfs_stash_unmount(mount_dir));

	TEST("change password", encfs_stash_change_password(crypt_dir, PASSWORD1, PASSWORD2, &output));
	OUTPUT();

	TEST("mount", encfs_stash_mount(crypt_dir, mount_dir, PASSWORD2, 0, &output));
	OUTPUT();
	TEST("unmount", encfs_stash_unmount(mount_dir));
	
	TEST("incorrect password mount", !encfs_stash_mount(crypt_dir, mount_dir, "blah", 0, &output));
	OUTPUT();

	system("touch unit_test_mount/file");
	TEST("non-empty mount point mount", !encfs_stash_mount(crypt_dir, mount_dir, PASSWORD2, 0, &output));
	OUTPUT();

	TEST("get_info", encfs_stash_get_info(crypt_dir, &output));
	OUTPUT();
	

	system("rm -rf unit_test_crypt unit_test_mount");

	printf("%d tests passed, %d tests failed\n", passed, failed);
}
