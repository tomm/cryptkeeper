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
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <assert.h>
#include <cstring>
#include <unistd.h>
#include "lsof.h"

fsuser::fsuser(const char *pid, const char *name)
{
	this->pid = atoi(pid);
	memset(this->name, 0, sizeof(this->name));
	strncpy(this->name, name, sizeof(this->name)-1);
}

void get_fsusers(lsof_result_t *result, const char *mount_dir)
{
	int fd[2];

	result->fsusers.clear();
	result->num.clear();

	assert(pipe(fd)==0);

	int pid = fork();
	if (pid==0) {
		dup2(fd[1],1);
		close(fd[0]);
		execlp("lsof","lsof","-Fc",mount_dir,NULL);
		exit(0);
	}
	close(fd[1]);
	char buf[256];
	FILE *f = fdopen(fd[0], "r");

	while (fgets(buf, sizeof(buf), f)) {
		buf[strlen(buf)-1] = 0;
		std::string _pid = &buf[1];
		fgets(buf, sizeof(buf), f);
		buf[strlen(buf)-1] = 0;
		std::string _name = &buf[1];

		result->fsusers.push_back(fsuser(_pid.c_str(), _name.c_str()));
		if (result->num.find(_name) != result->num.end()) {
			result->num[_name]++;
		} else {
			result->num[_name] = 1;
		}
		
		//msg += _name + " (pid " + _pid + ")\n";
	}
	fclose(f);
	close(fd[0]);
}

