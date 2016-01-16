#include <ctype.h>
#include "strncmp.h"

#ifndef HAVE_STRNICMP
#ifndef HAVE_STRNCASECMP

int _strnicmp(const char* s1, const char* s2, int len)
{
	for(;*s1 && len > 0;s1++, s2++, len--) {
		const char c1 = tolower(*s1);
		const char c2 = tolower(*s2);

		if (c1 < c2) return -1;
		else if (c1 > c2) return 1;
	}

	return 0;
}

#endif
#endif
