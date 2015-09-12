/* Copyright (c) 2007-2013 Antony Dovgal <tony@daylessday.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PINBA_H
#define PINBA_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

extern "C" {
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <Judy.h>
}

#include "pinba.pb-c.h"
#include "pinba_config.h"
#include "pinba_types.h"

#undef P_SUCCESS
#undef P_FAILURE
#define P_SUCCESS 0
#define P_FAILURE -1


#define P_ERROR        (1<<0L)
#define P_WARNING      (1<<1L)
#define P_NOTICE       (1<<2L)
#define P_DEBUG        (1<<3L)
#define P_DEBUG_DUMP   (1<<4L)

char *pinba_error_ex(int return_error, int type, const char *file, int line, const char *format, ...);

#define PINBA_DEBUG

#ifdef PINBA_DEBUG
#define pinba_debug(...) pinba_error_ex(0, P_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#else
#define pinba_debug(...)
#endif

#define pinba_warning(...) pinba_error_ex(0, P_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define pinba_error(type, ...) pinba_error_ex(0, type, __FILE__, __LINE__, __VA_ARGS__)
#define pinba_error_get(type, ...) pinba_error_ex(1, type, __FILE__, __LINE__, __VA_ARGS__)
//extern pinba_daemon *D;

#ifdef __GNUC__
#define LIKELY(x)       __builtin_expect((x),1)
#define UNLIKELY(x)     __builtin_expect((x),0)
#else
#define LIKELY(x)       x
#define UNLIKELY(x)     x
#endif

/* utility macros */

#define timeval_to_float(tv) ((float)(tv).tv_sec + ((float)(tv).tv_usec / 1000000.0))

static inline struct timeval float_to_timeval(double f)
{
	struct timeval t;
	double fraction, integral;

	fraction = modf(f, &integral);
	t.tv_sec = (int)integral;
	t.tv_usec = (int)(fraction*1000000);
	return t;
}

struct pinba_version_info {
	const char *vcs_date;
	const char *vcs_branch;
	const char *vcs_full_hash;
	const char *vcs_short_hash;
	const char *vcs_wc_modified;
};

#endif /* PINBA_H */
