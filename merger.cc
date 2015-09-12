#include <sys/time.h>
#include "report_list.h"
#include <unistd.h>
#include "report_manager.h"
#include "report_container.h"


void merge_loop(ReportManager *rpm, struct timeval *period)
{
	for (;;) {
		bool found = false;
		ReportList *main_list = rpm->get_main_list();
		for (size_t i = 0; i < rpm->get_list_count(); i++) {
			ReportContainer *reps = rpm->get_thread_container(i);
			ReportList *reports = reps->get_full_list();
			if (reports) {
				found = true;
				reports->merge_reports(main_list);
				reps->return_empty_list(reports);
			}
		}

		if (!found) {
			usleep(period->tv_sec * 1000000 + period->tv_usec);
#if 0
			Report *report = main_list->get_report("simple");
			if (report) {
				ReportData *data = report->get_report_data_first();
				if (data) {
					data->print_record(0);
					data->print_record(1);
				}
			}

#endif
		}

	}
}
