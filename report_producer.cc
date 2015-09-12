#include "reports.h"
#include "report_producer.h"

Report *ReportProducer::create_report() {
	return new Report();
}
