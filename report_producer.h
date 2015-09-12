#pragma once

#include "reports.h"

class ReportProducer {
public:
	ReportProducer() {};
	virtual Report *create_report();
};
