/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*
 * Copyright 2015 Lanet Network
 * Programmed by Oleksandr Natalenko <o.natalenko@lanet.ua>
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

#pragma once

#ifndef __PFGLFQ_H__
#define __PFGLFQ_H__

typedef void (*dentry_handler_t)(glfs_t*, const char*, struct dirent*, struct stat*, void*, unsigned int);
typedef int (*dentry_comparator_t)(const char*, struct dirent*, struct stat*, unsigned int);

void walk_dir_generic(glfs_t* _fs, const char* _entry_point, dentry_handler_t _handler, dentry_comparator_t _comparator, void* _data, unsigned int _level) __attribute__((nonnull(1, 2, 3)));

#endif /* __PFGLFQ_H__ */

