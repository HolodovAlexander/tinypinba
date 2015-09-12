#pragma once
#include <atomic>
#include <map>
#include <queue>
#include <mutex>
#include "reports.h"
#include "pinba.pb-c.h"
#include "report_producer.h"


class ReportList {
	std::map<std::string, Report *> reports;
public:
	unsigned req_count;
	unsigned version;
	ReportList() : req_count(0) {};
	virtual int add_report(Report *report, std::string report_type);
	virtual void del_report(std::string report_type);
	virtual void merge_reports(ReportList *main_reports);
	virtual Report *get_report(std::string report_type);
	int add_request_to_reports(Pinba__Request *request);
	int del_request_from_reports(Pinba__Request *request);
};

class SharedReportList : public ReportList {
	std::mutex lock;
public:
	SharedReportList() : ReportList() {};
	virtual int add_report(Report *report, std::string type) {
		std::lock_guard<std::mutex> lck(lock);
		return ReportList::add_report(report, type);	
	};

	virtual void del_report(std::string type) {
		std::lock_guard<std::mutex> lck(lock);
		ReportList::del_report(type);
	}
	virtual Report *get_report(std::string type) {
		lock.lock();
		return ReportList::get_report(type);
	}

	void return_report() {
		lock.unlock();
	}
};


