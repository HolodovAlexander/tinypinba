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

#include "pinba_limits.h"

#ifndef PINBA_TYPES_H
#define PINBA_TYPES_H

enum {
	PINBA_TABLE_UNKNOWN,
	PINBA_TABLE_STATUS, /* internal status table */
	PINBA_TABLE_REQUEST,
	PINBA_TABLE_TIMER,
	PINBA_TABLE_TIMERTAG,
	PINBA_TABLE_TAG,
	PINBA_TABLE_HISTOGRAM_VIEW,
	PINBA_TABLE_REPORT_INFO,
	PINBA_TABLE_REPORT1, /* group by script_name */
	PINBA_TABLE_REPORT2, /* group by virtual host */
	PINBA_TABLE_REPORT3, /* group by hostname */
	PINBA_TABLE_REPORT4, /* group by virtual host, script_name */
	PINBA_TABLE_REPORT5, /* group by hostname, script_name */
	PINBA_TABLE_REPORT6, /* group by hostname, virtual_host */
	PINBA_TABLE_REPORT7, /* group by hostname, virtual_host and script_name */
	PINBA_TABLE_REPORT8, /* group by status */
	PINBA_TABLE_REPORT9, /* group by script_name and status */
	PINBA_TABLE_REPORT10, /* group by virtual_host and status */
	PINBA_TABLE_REPORT11, /* group by hostname and status */
	PINBA_TABLE_REPORT12, /* group by hostname, script_name and status */
	PINBA_TABLE_REPORT13, /* group by schema */
	PINBA_TABLE_REPORT14, /* group by schema and script_name */
	PINBA_TABLE_REPORT15, /* group by schema and server_name */
	PINBA_TABLE_REPORT16, /* group by schema and hostname */
	PINBA_TABLE_REPORT17, /* group by schema, hostname and script_name */
	PINBA_TABLE_REPORT18, /* group by schema, hostname and status */
	PINBA_TABLE_TAG_INFO, /* tag report grouped by custom tag */
	PINBA_TABLE_TAG2_INFO, /* tag report grouped by 2 custom tags */
	PINBA_TABLE_TAGN_INFO, /* tag report grouped by N custom tags */
	PINBA_TABLE_TAG_REPORT, /* tag report grouped by script_name and custom tag */
	PINBA_TABLE_TAG2_REPORT, /* tag report grouped by script_name and 2 custom tags */
	PINBA_TABLE_TAGN_REPORT, /* tag report grouped by script_name and N custom tags */
	PINBA_TABLE_TAG_REPORT2, /* tag report grouped by script_name, host_name, server_name and custom tag */
	PINBA_TABLE_TAG2_REPORT2, /* tag report grouped by script_name, host_name, server_name and 2 custom tags */
	PINBA_TABLE_TAGN_REPORT2, /* tag report grouped by script_name, host_name, server_name and N custom tags */
	PINBA_TABLE_RTAG_INFO, /* request tag report grouped by 1 tag */
	PINBA_TABLE_RTAG2_INFO, /* request tag report grouped by 2 tags */
	PINBA_TABLE_RTAGN_INFO, /* request report grouped by N tags */
	PINBA_TABLE_RTAG_REPORT, /* request tag report grouped by 1 tag and hostname */
	PINBA_TABLE_RTAG2_REPORT, /* request tag report grouped by 2 tags and hostname */
	PINBA_TABLE_RTAGN_REPORT /* request report grouped by N tags and hostname */
};

#define PINBA_TABLE_REPORT_LAST PINBA_TABLE_REPORT18

enum {
	PINBA_REPORT_REGULAR = 1<<0,
	PINBA_REPORT_CONDITIONAL = 1<<1,
	PINBA_REPORT_TAGGED = 1<<2
};

enum {
	PINBA_BASE_REPORT_KIND = 0,
	PINBA_TAG_REPORT_KIND,
	PINBA_RTAG_REPORT_KIND
};

typedef struct _pinba_socket {
	int listen_sock;
} pinba_socket;


#endif /* PINBA_TYPES_H */

/*
 * vim600: sw=4 ts=4 fdm=marker
 */
