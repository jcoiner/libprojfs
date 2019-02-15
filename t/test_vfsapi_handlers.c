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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_common.h"

static int test_handle_event(const char *desc, const char *path,
			     int pid, const char *procname,
			     int isdir, int type, int proj)
{
	unsigned int opt_flags, ret_flags;
	const char *retfile;
	int ret;

	opt_flags = test_get_opts((TEST_OPT_VFSAPI | TEST_OPT_RETVAL
						   | TEST_OPT_RETFILE),
				  &ret, &ret_flags, &retfile);

	if ((opt_flags & TEST_OPT_RETFILE) == TEST_OPT_NONE ||
	    (ret_flags & TEST_FILE_EXIST) != TEST_FILE_NONE) {
		printf("  Test%s for %s: %d, %s, %hhd, 0x%08X\n",
		       desc, path, pid, procname, isdir, type);
	}

	if (proj) {
		if (!isdir) {
			fprintf(stderr, "unsupported file projection\n");
			ret = PrjFS_Result_Invalid;
		}
	}

	if ((ret_flags & TEST_VAL_SET) == TEST_VAL_UNSET)
		ret = PrjFS_Result_Success;

	test_free_opts();

	return ret;
}

static PrjFS_Result TestEnumerateDirectory(
    _In_    unsigned long                           commandId,
    _In_    const char*                             relativePath,
    _In_    int                                     triggeringProcessId,
    _In_    const char*                             triggeringProcessName
)
{
	(void)commandId;		// prevent compiler warnings

	return test_handle_event("EnumerateDirectory", relativePath,
				 triggeringProcessId, triggeringProcessName,
				 1, 0, 1);
}

static PrjFS_Result TestNotifyOperation(
    _In_    unsigned long                           commandId,
    _In_    const char*                             relativePath,
    _In_    unsigned char                           providerId[PrjFS_PlaceholderIdLength],
    _In_    unsigned char                           contentId[PrjFS_PlaceholderIdLength],
    _In_    int                                     triggeringProcessId,
    _In_    const char*                             triggeringProcessName,
    _In_    bool                                    isDirectory,
    _In_    PrjFS_NotificationType                  notificationType,
    _In_    const char*                             destinationRelativePath
)
{
	(void)commandId;		// prevent compiler warnings
	(void)providerId;
	(void)contentId;
	(void)destinationRelativePath;

	return test_handle_event("NotifyOperation", relativePath,
				 triggeringProcessId, triggeringProcessName,
				 isDirectory, notificationType, 0);
}

int main(int argc, char *const argv[])
{
	const char *lower_path, *mount_path;
	PrjFS_MountHandle *handle;
	PrjFS_Callbacks callbacks = { 0 };

	test_parse_mount_opts(argc, argv,
			      (TEST_OPT_VFSAPI | TEST_OPT_RETVAL
					       | TEST_OPT_RETFILE),
			      &lower_path, &mount_path);

	memset(&callbacks, 0, sizeof(PrjFS_Callbacks));
	callbacks.EnumerateDirectory = TestEnumerateDirectory;
	callbacks.NotifyOperation = TestNotifyOperation;

	test_start_vfsapi_mount(lower_path, mount_path, callbacks,
				0, &handle);
	test_wait_signal();
	test_stop_vfsapi_mount(handle);

	exit(EXIT_SUCCESS);
}

