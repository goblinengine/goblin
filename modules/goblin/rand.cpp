#include "rand.h"
#include "core/os/os.h"
#include "modules/regex/regex.h"

#include <stdlib.h>

Rand *Rand::singleton = nullptr;

Rand::Rand() {
	singleton = this;
}

Rand::~Rand() {}

int Rand::i(int from, int to) {
	randomize();
	return randi_range(from, to);
}

real_t Rand::f(real_t from, real_t to) {
	randomize();
	return randf_range(from, to);
}

int Rand::i_alt(int from, int to, int algorithm) {
	return int(abs(sin(OS::get_singleton()->get_ticks_usec() % to / 12.531)) * to * 11.31) % (to - from) + from;
}

Variant Rand::choice(const Variant &p_from) {
	switch (p_from.get_type()) {
		case Variant::STRING: {
			String str = p_from;
			ERR_FAIL_COND_V_MSG(str.empty(), Variant(), "String is empty.");
			return str.substr(rand() % str.length(), 1); // Not size().
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
		case Variant::ARRAY: {
			Array arr = p_from;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");
			return arr[rand() % arr.size()];
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = p_from;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");
			return dict.get_value_at_index(rand() % dict.size());
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Variant();
}

void Rand::shuffle(Array p_array) {
	if (p_array.size() < 2) {
		return;
	}
	for (int i = p_array.size() - 1; i > 0; --i) {
		const uint32_t j = rand() % (i + 1);
		const Variant tmp = p_array[i];
		p_array[i] = p_array[j];
		p_array[j] = tmp;
	}
}

bool Rand::decision(float probability) {
	return randbase.randf() <= probability;
}

Variant Rand::roll(uint32_t count, uint32_t sides) {
	if (sides > 144 || sides < 2) {
		ERR_FAIL_V_MSG(Variant(), "Sides must be between 2 and 144");
	}
	if (count < 1 || count > 100) {
		ERR_FAIL_V_MSG(Variant(), "Rolls must be between 1 and 100");
	}
	
	Array rolls = *new Array();
	uint32_t sum = 0;
	uint32_t val = 0;
	for (unsigned int i = 0; i < count; i++) {
		val = rand() % sides + 1;
		rolls.append(val);
		sum += val;
	}

	//add sum as first value
	rolls.insert(0, sum);
	return Variant(rolls);
}

Color Rand::color() {
	return Color();
}

void Rand::_bind_methods() { 
	ClassDB::bind_method(D_METHOD("ia", "from", "to", "algorithm"), &Rand::ia, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("i", "from", "to"), &Rand::i);
	ClassDB::bind_method(D_METHOD("f", "from", "to"), &Rand::f);

    ClassDB::bind_method(D_METHOD("choice", "from"), &Rand::choice);
	ClassDB::bind_method(D_METHOD("shuffle", "array"), &Rand::shuffle);
	ClassDB::bind_method(D_METHOD("decision", "probability"), &Rand::decision);
	ClassDB::bind_method(D_METHOD("roll", "count", "sides"), &Rand::roll);
}