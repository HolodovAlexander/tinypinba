#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include <sys/time.h>
#include "pinba.pb-c.h"
#include "report_data.h"
#include "dictionary.h"

const char *separator = "|";

class Report {
	// TODO string->char*
	std::map<std::string, ReportData*> report_data;
protected:
	size_t kbytest_total;
	size_t memory_footprint;
	struct timeval time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	virtual ReportData *create_new_data(ReportData *data);
	virtual ReportData *create_new_data(Pinba__Request *request);
public:
	virtual ~Report() {};
	virtual int add_record(Pinba__Request *request) = 0;
	virtual int del_record(Pinba__Request *request) = 0;
	virtual int merge_report(Report *report);
	virtual void clear();
	void add_report_data(const char *index, ReportData *data);
	ReportData *get_report_data(const char *index);
	ReportData *get_report_data_first();
	ReportData *get_report_data_next(const char *index);
};

class RequestReport : public Report {
	ReportData *get_or_create(Pinba__Request *request);
protected:
	virtual ReportData *create_new_data(ReportData *data);
	virtual ReportData *create_new_data(Pinba__Request *request, Dictionary *dict);
	virtual ReportData *get_data(Pinba__Request *request, Dictionary *dict);
public:
	virtual ~RequestReport() {};
	virtual int add_record(Pinba__Request *request);
	virtual int del_record(Pinba__Request *request);
	virtual bool is_suitable(Pinba__Request *request) { return true; };
};

class TimerReport : public Report {
public:
	virtual ~TimerReport() {};
	virtual int add_record(Pinba__Request *request);
	virtual int del_record(Pinba__Request *request);
};
