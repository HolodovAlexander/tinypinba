#pragma once

#include <sys/time.h>
#include "pinba.pb-c.h"

class ReportData {
	ssize_t req_count;
	struct timeval req_time_total;
	struct timeval ru_utime_total;
	struct timeval ru_stime_total;
	double kbytes_total;
	double memory_footprint;

public:
	ReportData() : req_count(0) { }
	virtual void add_record(Pinba__Request *request);
	virtual void del_record(Pinba__Request *request);
	virtual void merge_data(ReportData *data);
	virtual void clear();
	virtual bool is_empty();
	int print_record(size_t position);
};


