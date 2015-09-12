#include <sys/time.h>
#include "pinba.pb-c.h"
#include "reports.h"


void ReportData::add_record(Pinba__Request *request) {

	req_count++;

//	timeradd(&req_time_total, &record->data.req_time, &req_time_total);
//	timeradd(&ru_utime_total, &record->data.ru_utime, &ru_utime_total);
//	timeradd(&ru_stime_total, &record->data.ru_stime, &ru_stime_total);
	kbytes_total += request->document_size;
	memory_footprint += request->memory_footprint;
}

void ReportData::merge_data(ReportData *data)
{

	req_count += data->req_count;
	kbytes_total += data->kbytes_total;
	memory_footprint += data->memory_footprint;

	timeradd(&req_time_total, &data->req_time_total, &req_time_total);
	timeradd(&ru_utime_total, &data->ru_utime_total, &ru_utime_total);
	timeradd(&ru_stime_total, &data->ru_stime_total, &ru_stime_total);

}
