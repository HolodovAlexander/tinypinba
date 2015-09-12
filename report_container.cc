#include <mutex>
#include <deque>
#include "report_container.h"
#include "report_list.h"
#include "report_producer.h"

ReportContainer::ReportContainer(size_t chunks) : chunk_count(chunks) {
	report_chunks = new ReportList[chunks];
	for (size_t i = 0; i < chunks; i++) {
		empty_chunks.push_back(&report_chunks[i]);
	}
}

ReportContainer::~ReportContainer() {
	delete []report_chunks;

}

void ReportContainer::add_report(ReportProducer *producer, std::string type) {
	std::lock_guard<std::mutex> lck(cmd_lock);
	for (size_t i = 0; i < chunk_count; i++) {
		cmd_map[&report_chunks[i]].push_back({.producer = producer, .type = type});
	}
}

void ReportContainer::del_report(std::string type) {
	return add_report(NULL, type);
}

ReportList *ReportContainer::get_emty_list() {
	ReportList *reports = NULL;
	std::lock_guard<std::mutex> lck(chunk_lock);
	auto it = empty_chunks.begin();
	if (it != empty_chunks.end()) {
		reports = *it;
		empty_chunks.erase(it);
	}
	return reports;
}

ReportList *ReportContainer::get_full_list() {
	ReportList *reports = NULL;
	std::lock_guard<std::mutex> lck(chunk_lock);
	auto it = full_chunks.begin();
	if (it != full_chunks.end()) {
		reports = *it;
		full_chunks.erase(it);
	}
	return reports;
}

void ReportContainer::return_empty_list(ReportList *reports) {

	if (reports) {
		cmd_lock.lock();
		auto it = cmd_map[reports].begin();

		while (it != cmd_map[reports].end()) {
			if (it->producer) {
				Report *report = it->producer->create_report();
				reports->add_report(report, it->type);
			} else {
				reports->del_report(it->type);
			}

			cmd_map[reports].erase(it);
			it = cmd_map[reports].begin();
		}

		cmd_lock.unlock();
		std::lock_guard<std::mutex> lck(chunk_lock);
		empty_chunks.push_back(reports);
	}
}

void ReportContainer::return_full_list(ReportList *reports) {
	if (reports) {
		std::lock_guard<std::mutex> lck(chunk_lock);
		full_chunks.push_back(reports);
	}
}
