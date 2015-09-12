/* Copyright (c) 2007-2009 Antony Dovgal <tony@daylessday.org>

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

#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation        // gcc: Class implementation
#endif

#if defined(PINBA_ENGINE_DEBUG_ON) && !defined(DBUG_ON)
# undef DBUG_OFF
# define DBUG_ON
#endif

#if defined(PINBA_ENGINE_DEBUG_OFF) && !defined(DBUG_OFF)
# define DBUG_OFF
# undef DBUG_ON
#endif

#define MYSQL_SERVER 1
# include <include/mysql_version.h>
# include <sql/field.h>
# include <sql/structs.h>
# include <sql/handler.h>
#include <my_dir.h>
#include <mysql/plugin.h>
#include <mysql.h>
#include <my_pthread.h>

#include "ha_pinba.h"
# define pinba_free(a, b) my_free(a)

#ifndef hash_init
/* this is fucking annoying!
 * MySQL! or Sun! or Oracle! or whatever you're called this time of the day!
 * stop renaming the fucking functions and breaking the fucking API!
 */

# define hash_get_key    my_hash_get_key
# define hash_init       my_hash_init
# define hash_free       my_hash_free
# define hash_search     my_hash_search
# define hash_delete     my_hash_delete

#endif


/* Global variables */
static int port_var = 0;
static char *address_var = NULL;
static int data_pool_size_var = 0;
static int temp_pool_size_var = 0;
static int timer_pool_size_var = 0;
static int temp_pool_size_limit_var = 0;
static int request_pool_size_var = 0;
static int stats_history_var = 0;
static int stats_gathering_period_var = 0;
static int cpu_start_var = 0;
static int histogram_max_time_var = 0;
static int data_job_size_var = 0;

/* prototypes */
static handler* pinba_create_handler(handlerton *hton, TABLE_SHARE *table, MEM_ROOT *mem_root);
static void pinba_share_destroy(PINBA_SHARE *share);

/* Variables for pinba share methods */
static HASH pinba_open_tables; // Hash used to track open tables
pthread_mutex_t pinba_mutex;   // This is the mutex we use to init the hash


#define PINBA_TAG_PARAM_PREFIX "tag."
#define PINBA_TAG_PARAM_PREFIX_LEN strlen(PINBA_TAG_PARAM_PREFIX)

static void pinba_share_destroy(PINBA_SHARE *share)
{
}

static int free_share(PINBA_SHARE *share)
{
	pthread_mutex_lock(&pinba_mutex);
	if (!--share->use_count) {
		pinba_share_destroy(share);
		hash_delete(&pinba_open_tables, (unsigned char*) share);
		thr_lock_delete(&share->lock);
		pinba_free((unsigned char *) share, MYF(0));
	}
	pthread_mutex_unlock(&pinba_mutex);

	return 0;
}

static handler* pinba_create_handler(handlerton *hton, TABLE_SHARE *table, MEM_ROOT *mem_root)
{
	return new (mem_root) ha_pinba(hton, table);
}

ha_pinba::ha_pinba(handlerton *hton, TABLE_SHARE *table_arg) 
:handler(hton, table_arg)
{
	rec_buff = NULL;
	alloced_rec_buff_length = 0;
}

static const char *ha_pinba_exts[] = {
	NullS
};

const char **ha_pinba::bas_ext() const
{
	return ha_pinba_exts;
}

int ha_pinba::close(void) /* {{{ */
{
	DBUG_ENTER("ha_pinba::close");
	DBUG_RETURN(free_share(share));
}

int ha_pinba::rename_table(const char *from, const char *to) /* {{{ */
{
	return 0;
}

int ha_pinba::delete_table(const char *name) /* {{{ */
{
	return 0;
}

int ha_pinba::index_init(uint keynr, bool sorted)
{
	DBUG_ENTER("ha_pinba::index_init");
	active_index = keynr;

	if (active_index >= PINBA_MAX_KEYS) {
		DBUG_RETURN(HA_ERR_WRONG_INDEX);
	}

	this_index[active_index].position = 0;
	DBUG_RETURN(0);
}

int ha_pinba::index_read(unsigned char *buf, const unsigned char *key, uint key_len, enum ha_rkey_function find_flag) /* {{{ */
{
	DBUG_ENTER("ha_pinba::index_read");
	int ret;

	if (active_index >= PINBA_MAX_KEYS) {
		DBUG_RETURN(HA_ERR_WRONG_INDEX);
	}

	this_index[active_index].subindex.val = NULL;
	this_index[active_index].position = 0;
	ret = read_row_by_key(buf, active_index, key, key_len, 1);
	if (!ret) {
		this_index[active_index].position++;
	}
	DBUG_RETURN(ret);
}
/* }}} */

int ha_pinba::index_next(unsigned char *buf) /* {{{ */
{
	DBUG_ENTER("ha_pinba::index_next");
	int ret;

	if (active_index >= PINBA_MAX_KEYS) {
		DBUG_RETURN(HA_ERR_WRONG_INDEX);
	}

	ret = read_next_row(buf, active_index, true);
	if (!ret) {
		this_index[active_index].position++;
	}
	DBUG_RETURN(ret);
}
/* }}} */

int ha_pinba::index_prev(unsigned char *buf) /* {{{ */
{
	DBUG_ENTER("ha_pinba::index_prev");
	int ret;

	if (active_index >= PINBA_MAX_KEYS) {
		DBUG_RETURN(HA_ERR_WRONG_INDEX);
	}

	ret = read_next_row(buf, active_index, true);
	if (!ret) {
		this_index[active_index].position--;
	}
	DBUG_RETURN(ret);
}
/* }}} */

int ha_pinba::index_first(unsigned char *buf) /* {{{ */
{
	DBUG_ENTER("ha_pinba::index_first");
	int ret;

	if (active_index >= PINBA_MAX_KEYS) {
		DBUG_RETURN(HA_ERR_WRONG_INDEX);
	}

	this_index[active_index].position = 0;
	ret = read_index_first(buf, active_index);
	if (!ret) {
		this_index[active_index].position++;
	}
	DBUG_RETURN(ret);
}

int ha_pinba::rnd_init(bool scan) /* {{{ */
{
	int i;

	DBUG_ENTER("ha_pinba::rnd_init");

	for (i = 0; i < PINBA_MAX_KEYS; i++) {
		memset(&this_index[i], 0, sizeof(pinba_index_st));
	}

	switch (share->table_type) {
		case PINBA_TABLE_REQUEST:
		case PINBA_TABLE_TIMER:
		case PINBA_TABLE_TIMERTAG:
			this_index[0].ival = -1;
			this_index[0].position = -1;
			break;
	}

	DBUG_RETURN(0);
}

int ha_pinba::rnd_end() /* {{{ */
{
	DBUG_ENTER("ha_pinba::rnd_end");
	DBUG_RETURN(0);
}

int ha_pinba::rnd_next(unsigned char *buf) /* {{{ */
{
	int ret;

	DBUG_ENTER("ha_pinba::rnd_next");
	ret = read_next_row(buf, 0, false);
	DBUG_RETURN(ret);
}

int ha_pinba::rnd_pos(unsigned char * buf, unsigned char *pos) /* {{{ */
{
	int ret;
	DBUG_ENTER("ha_pinba::rnd_pos");
	ret = read_row_by_pos(buf, my_get_ptr(pos, ref_length));
	if (!ret) {
		this_index[active_index].position++;
	}
	DBUG_RETURN(ret);
}


int ha_pinba::read_row_by_pos(unsigned char *buf, my_off_t position) /* {{{ */
{
	DBUG_RETURN(HA_ERR_INTERNAL_ERROR);
}

int ha_pinba::read_next_row(unsigned char *buf, uint active_index, bool by_key)
{
	DBUG_ENTER("ha_pinba::read_next_row");
	int ret = HA_ERR_INTERNAL_ERROR;

	if (active_index >= PINBA_MAX_KEYS) {
		DBUG_RETURN(HA_ERR_WRONG_INDEX);
	}

	switch(share->table_type) {
		case PINBA_TABLE_REPORT1:
			ret = report1_fetch_row(buf);
			break;
		default:
			/* unsupported table type */
			ret = HA_ERR_INTERNAL_ERROR;
			goto failure;
	}

	if (ret == HA_ERR_KEY_NOT_FOUND) {
		ret = HA_ERR_END_OF_FILE;
	}

failure:
	table->status = ret ? STATUS_NOT_FOUND : 0;
	DBUG_RETURN(ret);
}
/* }}} */

void ha_pinba::position(const unsigned char *record)
{
	DBUG_ENTER("ha_pinba::position");
	DBUG_VOID_RETURN;
}

int ha_pinba::create(const char *name, TABLE *table_arg, HA_CREATE_INFO *create_info)
{
	DBUG_ENTER("ha_pinba::create");

	if (!table_arg->s) {
		DBUG_RETURN(HA_WRONG_CREATE_OPTION);
	}

	DBUG_RETURN(0);
}

int ha_pinba::delete_all_rows()
{
	DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}

int ha_pinba::info(uint flag)
{
	stats.records = 2;
	DBUG_RETURN(0);
}

THR_LOCK_DATA **ha_pinba::store_lock(THD *thd, THR_LOCK_DATA **to, enum thr_lock_type lock_type) /* {{{ */
{
	if (lock_type != TL_IGNORE && lock.type == TL_UNLOCK) {
		lock.type = lock_type;
	}
	*to++ = &lock;
	return to;
}

int ha_pinba::open(const char *name, int mode, uint test_if_locked) /* {{{ */
{
	DBUG_ENTER("ha_pinba::open");
	thr_lock_data_init(&share->lock, &lock, NULL);
	DBUG_RETURN(0);
}


static int pinba_engine_init(void *p) /* {{{ */
{
	handlerton *pinba_hton = (handlerton *)p;

	DBUG_ENTER("pinba_engine_init");

	(void)pthread_mutex_init(&pinba_mutex, MY_MUTEX_INIT_FAST);
	(void)hash_init(&pinba_open_tables, system_charset_info, 32, 0, 0, (hash_get_key)pinba_get_key, 0, 0);

	pinba_hton->state = SHOW_OPTION_YES;
	pinba_hton->create = pinba_create_handler;
	DBUG_RETURN(0);
}


/* conf variables {{{ */

static MYSQL_SYSVAR_INT(port,
  port_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "UDP port to listen at",
  NULL,
  NULL,
  30002,
  0,
  65536,
  0);

static MYSQL_SYSVAR_STR(address,
  address_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "IP address to listen at (leave it empty if you want to listen at any IP)",
  NULL,
  NULL,
  NULL);

static MYSQL_SYSVAR_INT(data_pool_size,
  data_pool_size_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Raw socket data pool size",
  NULL,
  NULL,
  0,
  0,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(temp_pool_size,
  temp_pool_size_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Temporary pool size",
  NULL,
  NULL,
  10000,
  10,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(temp_pool_size_limit,
  temp_pool_size_limit_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Temporary pool size limit",
  NULL,
  NULL,
  0,
  1000,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(timer_pool_size,
  timer_pool_size_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Timer pool size",
  NULL,
  NULL,
  100000,
  10,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(request_pool_size,
  request_pool_size_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Request pool size",
  NULL,
  NULL,
  1000000,
  10,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(stats_history,
  stats_history_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Request stats history (seconds)",
  NULL,
  NULL,
  900, /* 15 * 60 sec */
  1,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(stats_gathering_period,
  stats_gathering_period_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Request stats gathering period (microseconds)",
  NULL,
  NULL,
  10000,
  10,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(cpu_start,
  cpu_start_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Set CPU affinity offset",
  NULL,
  NULL,
  0,
  0,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(histogram_max_time,
  histogram_max_time_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Set max time value for median computation",
  NULL,
  NULL,
  10,
  1,
  INT_MAX,
  0);

static MYSQL_SYSVAR_INT(data_job_size,
  data_job_size_var,
  PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_READONLY,
  "Set the number of packets in the queue to start processing",
  NULL,
  NULL,
  2048,
  1,
  INT_MAX,
  0);

static struct st_mysql_sys_var* system_variables[]= {
	MYSQL_SYSVAR(port),
	MYSQL_SYSVAR(address),
	MYSQL_SYSVAR(data_pool_size),
	MYSQL_SYSVAR(timer_pool_size),
	MYSQL_SYSVAR(temp_pool_size),
	MYSQL_SYSVAR(temp_pool_size_limit),
	MYSQL_SYSVAR(request_pool_size),
	MYSQL_SYSVAR(stats_history),
	MYSQL_SYSVAR(stats_gathering_period),
	MYSQL_SYSVAR(cpu_start),
	MYSQL_SYSVAR(histogram_max_time),
	MYSQL_SYSVAR(data_job_size),
	NULL
};

struct st_mysql_storage_engine pinba_storage_engine =
{ MYSQL_HANDLERTON_INTERFACE_VERSION };

mysql_declare_plugin(pinba) /* {{{ */
{
	MYSQL_STORAGE_ENGINE_PLUGIN,
	&pinba_storage_engine,
	"PINBA",
	"Antony Dovgal",
	"Pinba engine",
	PLUGIN_LICENSE_GPL,
	pinba_engine_init,          /* Plugin Init */
	pinba_engine_shutdown,      /* Plugin Deinit */
	0x0101, /* VERSION 1.1.0 */
	NULL,                       /* status variables                */
	system_variables,           /* system variables                */
	NULL                        /* config options                  */
}
mysql_declare_plugin_end;
/* }}} */

/*
 * vim600: sw=4 ts=4 fdm=marker
 */
