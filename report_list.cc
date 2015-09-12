#include <iostream>
#include "report_list.h"
#include "pinba.h"
#include "pinba.pb-c.h"

void ReportList::merge_reports(ReportList *main_reports)
{
	for (auto it = reports.begin(); it != reports.end(); ++it) {
		Report *report = it->second;
		if (report) {
			auto main_rep = main_reports->reports.find(it->first);
			if (main_rep == main_reports->reports.end()) {
				pinba_error(P_WARNING, "no report in main reports list");
				continue;
			}

			main_rep->second->merge_report(report);
			report->clear();
		}
	}
}


int ReportList::add_request_to_reports(Pinba__Request *request) {

	for (auto it = reports.begin(); it != reports.end(); ++it) {
		Report *report = it->second;
		if (report) {
			report->add_record(request);
		}
	}
	return 0;
}

int ReportList::del_request_from_reports(Pinba__Request *request) {

	for (auto it = reports.begin(); it != reports.end(); ++it) {
		Report *report = it->second;
		if (report) {
			report->del_record(request);
		}
	}
	return 0;
}

void ReportList::del_report(std::string type) {
	auto it = reports.find(type);
	if (it != reports.end()) {
		delete it->second;
		reports.erase(type);
	}
}

int ReportList::add_report(Report *report, std::string type) {
	std::cout << "try to add report " << type << " to list" << std::endl;
	auto it = reports.find(type);
	if (it != reports.end()) {
		std::cout << "report " << type <<  " alread exist, skip" << std::endl;
		return -1;
	}
	reports[type] = report;
	return 0;
}

Report *ReportList::get_report(std::string type) {
	auto it = reports.find(type);
	if (it == reports.end()) {
		return NULL;
	}

	return it->second;
}
