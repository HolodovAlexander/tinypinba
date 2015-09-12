#pragma once
#include <string>
#include <unordered_map>
#include "pinba.pb-c.h"

class Dictionary {
	std::unordered_map<unsigned, std::string> dict;
	std::unordered_map<std::string, unsigned> inv_dict;
	size_t cur_pos;
public:
	Dictionary() : cur_pos(0) {};
	void new_request(Pinba__Request *request);
	const char *get_string(unsigned index);
	unsigned add_word(const char *string);
};
