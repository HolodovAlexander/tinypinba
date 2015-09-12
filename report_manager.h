#pragma once
#include <string>
#include "report_list.h"
#include "report_producer.h"
#include "report_container.h"

class ReportManager {
	size_t count;
	ReportContainer* thread_containers;
	SharedReportList *main_list;
public:
	ReportManager(size_t thread_count);
	~ReportManager();
	void add_new_report(ReportProducer *report_produser, std::string report_type);
	void delete_report(std::string);
	ReportContainer *get_thread_container(size_t thread_num);
	size_t get_list_count() { return count; };
	SharedReportList *get_main_list() { return main_list; };
	Report *get_report(std::string report_type);
	void return_report(Report *r);
};

