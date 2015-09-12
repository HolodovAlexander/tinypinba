#include "pinba.pb-c.h"

int pinba_request_fill_dictionary(Pinba__Request *request, PinbaDict *dict) {
	for (size_t i = 0; i < request->n_dictionary; i++) {
		const char *str = request->dictionary + PINBA_DICTIONARY_ENTRY_SIZE * i;
		dict->add_word(i, str);
	}
}

const char *pinba_request_get_tag(Pinba__Request *request, PinbaDict *dict, const char *tag_name) {

	for (size_t i = 0; i < request->n_tag_name; i++) {
//		request->tag_name
//		if (memcmp(tag_name, request->ta
	}
}
