/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*
 * Copyright 2015 Lanet Network
 * Programmed by Oleksandr Natalenko <o.natalenko@lanet.ua>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <glusterfs/api/glfs.h>
#include <pfcq.h>
#include <pfgfq.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

static pthread_mutex_t glfs_mkdir_safe_lock = PTHREAD_MUTEX_INITIALIZER;

int glfs_mkdir_safe(glfs_t* _fs, const char* _path, mode_t _mode)
{
	int ret = 0;

	if (unlikely(pthread_mutex_lock(&glfs_mkdir_safe_lock)))
		panic("pthread_mutex_lock");
	ret = glfs_mkdir(_fs, _path, _mode);
	if (unlikely(pthread_mutex_unlock(&glfs_mkdir_safe_lock)))
		panic("pthread_mutex_unlock");

	return ret;
}

void walk_dir_generic(glfs_t* _fs, const char* _entry_point, dentry_handler_t _handler, dentry_comparator_t _comparator, void* _data, unsigned int _level)
{
	glfs_fd_t* fd = NULL;
	struct dirent* entry = NULL;
	char path[PATH_MAX];
	struct stat sb;

	pfcq_zero(&sb, sizeof(struct stat));
	pfcq_zero(path, PATH_MAX);

	debug("Walking into %s entry...\n", _entry_point);

	fd = glfs_opendir(_fs, _entry_point);
	if (unlikely(!fd))
	{
		warning("glfs_opendir");
		goto out;
	}

	while (likely(entry = glfs_readdirplus(fd, &sb)))
	{
		if (unlikely(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0))
			continue;

		if (likely(_comparator))
			if (unlikely(!_comparator(_entry_point, entry, &sb, _level)))
				continue;

		if (likely(strcmp(_entry_point, "/") == 0))
		{
			if (unlikely(snprintf(path, PATH_MAX, "/%s", entry->d_name) < 0))
				continue;
		} else
		{
			if (unlikely(snprintf(path, PATH_MAX, "%s/%s", _entry_point, entry->d_name) < 0))
				continue;
		}
		if (likely(_handler))
			_handler(_fs, path, entry, &sb, _data, _level);
	}

	if (unlikely(glfs_closedir(fd)))
		warning("glfs_closedir");

out:
	debug("Walking out of %s entry...\n", _entry_point);

	return;
}

