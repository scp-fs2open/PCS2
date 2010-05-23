#ifndef FILELENGTH_INCLUDED
#define FILELENGTH_INCLUDED

#ifdef WIN32 
#define HAVE_FILELENGTH
#endif
/* define a portable layer for filelength() DOS function */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef HAVE_FILELENGTH

long filelength(int fd);

#else /* !HAVE_FILELENGTH */

#include <io.h>

#endif

#endif /* FILELENGTH_INCLUDED */
