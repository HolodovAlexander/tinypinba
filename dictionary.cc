#include "dictionary.h"

void Dictionary::new_request(Pinba__Request *request) {
	for (size_t i = 0; i < request->n_dictionary; i++) {
		const char *str = request->dictionary + PINBA_DICTIONARY_ENTRY_SIZE * i;
		unsigned index = add_word(str);
		for (size_t j = 0; j < request->n_tag_name; j++) {
			if (request->tag_name[j] == i) {
				request->tag_name[j] = index | 0x80000000;
			}
		}

		for (size_t j = 0; j < request->n_tag_value; j++) {
			if (request->tag_value[j] == i) {
				request->tag_value = index | 0x80000000;
			}
		}

	}
}
