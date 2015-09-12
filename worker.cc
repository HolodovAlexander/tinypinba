#include <list>
#include "pinba.h"
#include "pinba.pb-c.h"
#include "report_list.h"
#include "report_container.h"

int request_check(Pinba__Request *request) {
	if (!request) {
		return -1;
	}

	size_t timers_cnt = request->n_timer_hit_count;
	if (timers_cnt != request->n_timer_value || timers_cnt != request->n_timer_tag_count) {
		pinba_error(P_WARNING, "malformed data: timer_hit_count_size != timer_value_size || timer_hit_count_size != timer_tag_count_size");
		return -1;
	}

	if (request->n_tag_name != request->n_tag_value) {
		pinba_error(P_WARNING, "malformed data: n_tag_name != n_tag_value");
		return -1;
	}

	if (request->n_dictionary == 0) {
		if (timers_cnt > 0) {
			pinba_error(P_WARNING, "malformed data: dict_size == 0, but timers_cnt > 0");
			return -1;
		}
		if (request->n_tag_name > 0) {
			pinba_error(P_WARNING, "malformed data: dict_size == 0, but tags are present");
			return -1;
		}
	}

	return 0;
}

int add_request_to_reports(Pinba__Request *request, ReportList *reports)
{
	if (request && request_check(request) == 0) {
		reports->add_request_to_reports(request);
	}

	for (size_t i = 0; i < request->n_requests; i++) {
		add_request_to_reports(request->requests[i], reports);
	}	

	return 0;
}

void pinba_worker_loop(pinba_socket *sock, ReportContainer *reps, struct timeval *period)
{
	std::list<Pinba__Request*> requests;

	ReportList *reports = NULL;
	struct timeval next = {0, 0};

	for (;;) {

		struct timeval cur;
		gettimeofday(&cur, 0);

		if (timercmp(&next, &cur, <)) {
			ReportList *new_list = reps->get_emty_list();
			if (new_list) {
				if (reports) {
					reps->return_full_list(reports);
				}
				reports = new_list;
				timeradd(&cur, period, &next);
			}
		}

		unsigned char buf[PINBA_UDP_BUFFER_SIZE];
		int ret = recv(sock->listen_sock, buf, PINBA_UDP_BUFFER_SIZE, 0);
		if (ret > 0) {

			Pinba__Request *request = pinba__request__unpack(NULL, ret, buf);
			if (!request) {	
				continue;
			}

			if (reports) {
				add_request_to_reports(request, reports);
			}

			pinba__request__free_unpacked(request, NULL);
	//		requests.push_back(request);

		} else if (ret < 0) {
			if (errno == EINTR) {
				continue;
			}
			pinba_error(P_WARNING, "recv() failed: %s (%d)", strerror(errno), errno);
		} else {
			pinba_error(P_WARNING, "recv() returned 0");
		}


		// TODO: check for delete

	}
}


