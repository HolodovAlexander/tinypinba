#include "rtag_report1.h"

#if 0
ReportData *Rtag_report1::create_new_data(Pinba__Request *request, Dictionary *dict) {
	const char *tag_value = pinba_request_get_tag(request, tag_name.c_str());
	if (tag_value) {
		return new TagData(tag_value, request->hostname);
	}

	return NULL;
}
#endif

ReportData *Rtag_report1::create_new_data(ReportData *data) {
	TagData *tdata = dynamic_cast<TagData*>(data);
	if (tdata) {
		return new TagData(tdata->tag(), tdata->host(), tdata->get_tag_num());
	}
	return NULL;
}

ReportData *Rtag_report1::get_data(Pinba__Request *request, Dictionary *dict) {
//	int found = pinba_request_get_tag(request, tex
}
#if 0
        virtual ReportData *create_new_data(Pinba__Request *request, Dictionary *dict);
	          virtual ReportData *get_data(Pinba__Request *request, Dictionary *dict);
#endif
#if 0
char *Rtag_report1::get_index(Pinba__Request *request) {
	const char *tag_value = pinba_request_get_tag(request, tag_name.c_str());
	if (tag_value) {
		size_t max_len = strlen(request->hostname) + strlen(tag_value) + 2;
		char *index = (char*) malloc(max_len);
		snprintf(index, max_len, "%s|%s", request->hostname, tag_value);
		return index;
	}
	return NULL;
}
#endif


