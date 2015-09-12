#include "reports.h"
#include "pinba.pb-c.h"

void Report::clear() {
	for (auto it = report_data.begin(); it != report_data.end(); ++it) {
		it->second->clear();
	}
}


int Report::merge_report(Report *report) {
	for (auto it = report->report_data.begin(); it != report->report_data.end(); ++it) {
		auto my_it = report_data.find(it->first);
		if (my_it == report_data.end()) {
			if (!it->second->is_empty())  {
				ReportData *data = create_new_data(it->second);
				add_report_data(it->first.c_str(), data);
				data->merge_data(it->second);
			}
		} else {
			my_it->second->merge_data(it->second);
		}
	}
	return 0;
}

ReportData *Report::get_report_data(const char *index) {

	if (index == NULL) {
		return NULL;
	}

	auto it = report_data.find(index);
	if (it == report_data.end()) {
		return NULL;
	}

	return it->second;
}

ReportData *Report::get_report_data_first() {
	auto it = report_data.begin();

	if (it == report_data.end()) {
		return NULL;
	}

	return it->second;
}

ReportData *Report::get_report_data_next(const char *index) {

	auto it = report_data.upper_bound(index);
	if (it == report_data.end()) {
		return NULL;
	}

	return it->second;
}
