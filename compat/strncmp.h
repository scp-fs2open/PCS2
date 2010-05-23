#ifndef STRNCMP_INCLUDED
#define STRNCMP_INCLUDED

	/* offer a portable layer for _strnicmp() */

	#ifdef WIN32		/* presume that WIN32 defined means always have strncmp() and _strnicmp()
		 * declared into string.h (FIXME: is this always true ?) */
		# include <string.h>
	#else

		#ifdef HAVE_CONFIG_H
			# include <config.h>
		#endif

		#ifdef HAVE_STRINGS_H
			# include <strings.h>
		#endif

		#ifdef HAVE_STRING_H
			# include <string.h>
		#endif

		#ifndef HAVE_STRNICMP
			#ifdef HAVE_STRNCASECMP	/* maybe we have the other popular name for it */
				#define _strnicmp(s1,s2,len) strncasecmp(s1,s2,len)
			#else /* no option, use our own implementation of it */
				int _strnicmp(const char* s1, const char* s2, int len);
			#endif /* HAVE_STRNCASECMP */
		#endif /* !HAVE_STRNICMP */
	#endif /* WIN32 */
#endif /* STRNCMP_INCLUDED */