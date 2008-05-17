#ifndef _LSOF_H
#define _LSOF_H

#include <vector>
#include <map>

class fsuser {
	public:
	char name[64];
	int pid;
	fsuser(const char *pid, const char *name);
};

class lsof_result_t {
	public:
	std::vector<fsuser> fsusers;
	std::map<std::string, int> num;
};

void get_fsusers(lsof_result_t *result, const char *mount_dir);

#endif /* _LSOF_H */
