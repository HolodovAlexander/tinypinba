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

#include "pinba.h"

#include <sys/types.h>
#include <sys/socket.h>
#include"report_manager.h"
#include <pthread.h>
#include <string>
#include <iostream>

time_t last_error_time;
char last_errormsg[PINBA_ERR_BUFFER];

pthread_mutex_t error_mutex = PTHREAD_MUTEX_INITIALIZER;

char *pinba_error_ex(int return_error, int type, const char *file, int line, const char *format, ...)
{
	va_list args;
	const char *type_name;
	char *tmp;
	char tmp_format[PINBA_ERR_BUFFER/2];
	char errormsg[PINBA_ERR_BUFFER];

	switch (type) {
		case P_DEBUG_DUMP:
			type_name = "debug dump";
			break;
		case P_DEBUG:
			type_name = "debug";
			break;
		case P_NOTICE:
			type_name = "notice";
			break;
		case P_WARNING:
			type_name = "warning";
			break;
		case P_ERROR:
			type_name = "error";
			break;
		default:
			type_name = "unknown error";
			break;
	}

	snprintf(tmp_format, PINBA_ERR_BUFFER/2, "[PINBA] %s: %s:%d %s", type_name, file, line, format);

	va_start(args, format);
	vsnprintf(errormsg, PINBA_ERR_BUFFER, tmp_format, args);
	va_end(args);

	if (!return_error) {
		time_t t;
		struct tm *tmp;
		char timebuf[256] = {0};

		pthread_mutex_lock(&error_mutex);
		t = time(NULL);
		if ((t - last_error_time) < 1 && strcmp(last_errormsg, errormsg) == 0) {
			/* don't flood the logs */
			pthread_mutex_unlock(&error_mutex);
			return NULL;
		}
		last_error_time = t;
		strncpy(last_errormsg, errormsg, PINBA_ERR_BUFFER);

		tmp = localtime(&t);

		if (tmp) {
			strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tmp);
			fprintf(stderr, "[%s] %s\n", timebuf, errormsg);
		} else {
			fprintf(stderr, "%s\n", errormsg);
		}
		fflush(stderr);
		pthread_mutex_unlock(&error_mutex);
		return NULL;
	}
	tmp = strdup(errormsg);
	return tmp;
}

void pinba_socket_free(pinba_socket *socket)
{
	if (!socket) {
		return;
	}

	if (socket->listen_sock >= 0) {
		close(socket->listen_sock);
		socket->listen_sock = -1;
	}

	free(socket);
}

pinba_socket *pinba_socket_open(const char *ip, int listen_port)
{
	struct sockaddr_in addr;
	pinba_socket *s;
	int sfd, yes = 1;

	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		pinba_error(P_ERROR, "socket() failed: %s (%d)", strerror(errno), errno);
		return NULL;
	}

	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		close(sfd);
		return NULL;
	}

	s = (pinba_socket *)calloc(1, sizeof(pinba_socket));
	if (!s) {
		return NULL;
	}
	s->listen_sock = sfd;

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(listen_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (ip && *ip) {
		struct in_addr tmp;

		if (inet_aton(ip, &tmp)) {
			addr.sin_addr.s_addr = tmp.s_addr;
		} else {
			pinba_error(P_WARNING, "inet_aton(%s) failed, listening on ANY IP-address", ip);
		}
	}

	if (bind(s->listen_sock, (struct sockaddr *)&addr, sizeof(addr))) {
		pinba_socket_free(s);
		pinba_error(P_ERROR, "bind() failed: %s (%d)", strerror(errno), errno);
		return NULL;
	}

	return s;
}


void pinba_worker_loop(pinba_socket *sock, ReportContainer *reps, struct timeval *period);
void merge_loop(ReportManager *rpm, struct timeval *period);

typedef struct {
	ReportManager *rpm;
	struct timeval *period;
} merge_arg_t;

void *merge_thread_fnc(void *arg)  {
	merge_arg_t *merge_arg = (merge_arg_t*)arg;
	merge_loop(merge_arg->rpm, merge_arg->period);
	return NULL;
}

typedef struct {
	ReportContainer *reports;
	pinba_socket *s;
	struct timeval *period;
} worker_arg_t;


void *worker_thread_fnc(void *arg) {
	worker_arg_t *worker_arg = (worker_arg_t*)arg;
	pinba_worker_loop(worker_arg->s, worker_arg->reports, worker_arg->period);
	return NULL;
}

#define WORKER_CNT 5

int main()
{
	pinba_socket *pinba_socket = pinba_socket_open("127.0.0.1", 44001);
	if (!pinba_socket) {
		exit(-1);
	}

	pthread_t merge_thread;
	pthread_t worker_thread[WORKER_CNT];

	struct timeval period = {1, 0};
	ReportManager *rpm = new ReportManager(WORKER_CNT);

	for (size_t i = 0; i < WORKER_CNT; i++) {
		worker_arg_t *arg = (worker_arg_t*)calloc(1, sizeof(worker_arg_t));
		arg->reports = rpm->get_thread_container(i);
		arg->period = &period;
		arg->s = pinba_socket;
		if (pthread_create(&worker_thread[i], NULL, worker_thread_fnc, arg)) {
			return -1;
		}
	}

	merge_arg_t merge_arg = {
		.rpm = rpm,
		.period = &period,
	};

	if (pthread_create(&merge_thread, NULL, merge_thread_fnc, &merge_arg)) {
		return -1;
	}

	ReportProducer base_report;
	while (1) {
		sleep(1);
		std::string name;
		std::cin >> name;
		printf("%s\n", name.c_str());
		Report *report = rpm->get_report(name);
		if (!report) {
			rpm->return_report(report);
			rpm->add_new_report(&base_report, name);
		} else {
			ReportData *rdata = report->get_report_data_first();
			while (rdata) {
				size_t i = 0;
				while (rdata->print_record(i++) == 0) ;

				rdata = report->get_report_data_next(rdata->get_index());
			}
			rpm->return_report(report);
		}
	}
	return 0;

}
#if 0
	ReportProducer base_report;
	ReportContainer *reps = new ReportContainer(2);
	reps->add_report(&base_report, "simple");

	ReportList *main_list = new ReportList();
	main_list->add_report(base_report.create_report(), "simple");
#endif

