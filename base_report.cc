#include <sys/time.h>
#include <stdlib.h>
#include "pinba.pb-c.h"
#include "reports.h"

char *RequestReport::get_index(Pinba__Request *request) {
	return strdup(request->server_name);
}

ReportData *RequestReport::get_or_create(Pinba__Request *request, const char *index) {
	if (index) {
		ReportData *data = get_report_data(index);
		if (!data) {
			data = create_new_data(request, index);
		}

		return data;
	}
	return NULL;
}

int RequestReport::add_record(Pinba__Request *request) {

// TODO: common report times
	const char *index = get_index(request);
	int result = -1;

	if (index) {
		ReportData *data = get_or_create(request, index);
		if (data) {
			data->add_record(request, index);
			result = 0;
		}
	}
	free(index);
	return result;
}

int RequestReport::del_record(Pinba__Request *request) {

// TODO: common report times

	const char *index = get_index(request);
	int result = -1;

	if (index) {
		ReportData *data = get_or_create(request);
		if (data) {
			data->del_record(request);
			result = 0;
		}
	}

	free(index);
	return result;
}
