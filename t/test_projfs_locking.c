/* Linux Projected Filesystem
   Copyright (C) 2018-2019 GitHub, Inc.

   See the NOTICE file distributed with this library for additional
   information regarding copyright ownership.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library, in the file COPYING; if not,
   see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test_common.h"

#define LOCKFILE "test_projfs_locking.lock"

static int test_proj_event(struct projfs_event *event)
{
	int fd, res;

	if (event->mask != (PROJFS_CREATE_SELF | PROJFS_ONDIR))
		return -EINVAL;

	fd = open(LOCKFILE, (O_CREAT | O_EXCL | O_RDWR), 0600);
	if (fd == -1 && errno == EEXIST)
		return -EEXIST;
	else if (fd == -1)
		return -EINVAL;

	sleep(1);
	close(fd);

	res = unlink(LOCKFILE);
	if (res == -1)
		return -EINVAL;

	return 0;
}

int main(int argc, char *const argv[])
{
	const char *lower_path, *mount_path;
	struct projfs *fs;
	struct projfs_handlers handlers = { 0 };

	test_parse_mount_opts(argc, argv, 0, &lower_path, &mount_path);

	handlers.handle_proj_event = &test_proj_event;

	/* unlink if exists */
	unlink(LOCKFILE);

	fs = test_start_mount(lower_path, mount_path,
			      &handlers, sizeof(handlers), NULL);
	test_wait_signal();
	test_stop_mount(fs);

	exit(EXIT_SUCCESS);
}

