#include <iostream>
#include "reports.h"
#include "report_data.h"

bool ReportData::is_empty() {
	return (req_count == 0);
}

int ReportData::print_record(size_t position) {
	if (position == 1) {
		std::cout << req_count << std::endl;		
	} else {
		return -1;
	}

	return 0;
}

void ReportData::merge_data(ReportData *mdata) {
	req_count += mdata->req_count;
}

void ReportData::add_record(Pinba__Request *request) {
	req_count++;
}

void ReportData::del_record(Pinba__Request *request) {
	req_count--;
}

void ReportData::clear() {
	req_count = 0;
}

