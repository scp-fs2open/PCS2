#include "filelength.h"

#ifndef HAVE_FILELENGTH

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

long filelength(int fd)
{
	struct stat stbuf;

	if (fstat(fd, &stbuf)) /* error, how are those treated in PCS2 ?? */
		return 0;

	return stbuf.st_size;
}

#endif /* !HAVE_FILELENGTH */
