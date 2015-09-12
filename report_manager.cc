#include "report_manager.h"
#include "report_list.h"
#include "reports.h"
#include <vector>

ReportManager::ReportManager(size_t thread_count) : count(thread_count)  {
	thread_containers = new ReportContainer[thread_count];
	main_list = new SharedReportList();
}

ReportManager::~ReportManager() {
	delete []thread_containers;
}

void ReportManager::add_new_report(ReportProducer *report_produser, std::string report_type)
{
	for (size_t i = 0; i < count; i++) {
		thread_containers[i].add_report(report_produser, report_type);
	}

	Report *report = report_produser->create_report();
	if (report) {
		main_list->add_report(report, report_type);
	}
}

void ReportManager::delete_report(std::string report_type) {
	return add_new_report(NULL, report_type);
	main_list->del_report(report_type);
}

ReportContainer *ReportManager::get_thread_container(size_t thread_num) {
	if (thread_num >= count) {
		return NULL;
	}

	return &thread_containers[thread_num];
}

Report *ReportManager::get_report(std::string report_type) {
	return main_list->get_report(report_type);
}

void ReportManager::return_report(Report *r) {
	main_list->return_report();
}
