#pragma once
#include <deque>
#include <map>
#include "report_list.h"
#include "report_producer.h"
#include "dictionary.h"

class ReportContainer {
	std::deque<ReportList *> empty_chunks;
	std::deque<ReportList *> full_chunks;
	std::mutex chunk_lock;

	ReportList *report_chunks;
	size_t chunk_count;

	typedef struct {
		ReportProducer *producer;
		std::string  type;
	} report_cmd_t;

	std::map<ReportList *, std::deque<report_cmd_t> > cmd_map;
	std::mutex cmd_lock;

public:
	ReportContainer(size_t chunks = 2);
	~ReportContainer();
	
	ReportList *get_emty_list();
	ReportList *get_full_list();
	void return_full_list(ReportList *);
	void return_empty_list(ReportList *);
	void add_report(ReportProducer *report_producer, std::string report_type);
	void del_report(std::string report_type);
};

