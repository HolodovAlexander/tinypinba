/* request tag report grouped by 1 tag and hostname */

#include <string>
#include "reports.h"
#include "pinba.pb-c.h"

class TagData : public ReportData {
	std::string tag_value;
	std::string host_name;
	unsigned tag_num;
	std::string index;
public:
	TagData(const char *tag, const char *host, unsigned _num) : ReportData(), tag_value(tag), host_name(host), tag_num(_num) 
	{
		index = host_name + std::string(separator) + tag_value;
	}
	const char *tag() {return tag_value.c_str();}
	const char *host() {return host_name.c_str();}
	unsigned get_tag_num() {return tag_num;}
};


class Rtag_report1 : public RequestReport {
	std::string tag_name;
protected:
	virtual ReportData *create_new_data(ReportData *data);
	virtual ReportData *create_new_data(Pinba__Request *request, Dictionary *dict);
	virtual ReportData *get_data(Pinba__Request *request, Dictionary *dict);
public:
	Rtag_report1(const char *tag) : RequestReport(), tag_name(tag) {}
};

