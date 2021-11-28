/*************************************************************************/
/*  random_number_generator.cpp                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "random_number_generator.h"

RandomNumberGenerator::RandomNumberGenerator() {}

Variant RandomNumberGenerator::randchoice(const Variant &p_from) {
	switch (p_from.get_type()) {
		case Variant::STRING: {
			String str = p_from;
			ERR_FAIL_COND_V_MSG(str.empty(), Variant(), "String is empty.");
			return str.substr(randbase.rand() % str.length(), 1); // Not size().
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
			return arr[randbase.rand() % arr.size()];
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = p_from;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");
			return dict.get_value_at_index(randbase.rand() % dict.size());
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Variant();
}

void RandomNumberGenerator::randshuffle(Array p_array) {
	if (p_array.size() < 2) {
		return;
	}
	for (int i = p_array.size() - 1; i > 0; --i) {
		const uint32_t j = randbase.rand() % (i + 1);
		const Variant tmp = p_array[i];
		p_array[i] = p_array[j];
		p_array[j] = tmp;
	}
}

Variant RandomNumberGenerator::randroll(uint32_t count, uint32_t sides) {
	if (sides > 144 || sides < 2) {
		ERR_FAIL_V_MSG(Variant(), "Die sides must be between 2 and 144");
	}
	if (count < 1 || count > 100) {
		ERR_FAIL_V_MSG(Variant(), "Dice rolls must be between 1 and 100");
	}
	
	Array rolls = *new Array();
	uint32_t sum = 0;
	uint32_t val = 0;
	for (unsigned int i = 0; i < count; i++) {
		val = randbase.rand() % sides + 1;
		rolls.append(val);
		sum += val;
	}

	//add sum as first value
	rolls.insert(0, sum);
	return Variant(rolls);
}

void RandomNumberGenerator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_seed", "seed"), &RandomNumberGenerator::set_seed);
	ClassDB::bind_method(D_METHOD("get_seed"), &RandomNumberGenerator::get_seed);

	ClassDB::bind_method(D_METHOD("set_state", "state"), &RandomNumberGenerator::set_state);
	ClassDB::bind_method(D_METHOD("get_state"), &RandomNumberGenerator::get_state);

	ClassDB::bind_method(D_METHOD("randi"), &RandomNumberGenerator::randi);
	ClassDB::bind_method(D_METHOD("randf"), &RandomNumberGenerator::randf);
	ClassDB::bind_method(D_METHOD("randfn", "mean", "deviation"), &RandomNumberGenerator::randfn, DEFVAL(0.0), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("randf_range", "from", "to"), &RandomNumberGenerator::randf_range);
	ClassDB::bind_method(D_METHOD("randi_range", "from", "to"), &RandomNumberGenerator::randi_range);
	ClassDB::bind_method(D_METHOD("randomize"), &RandomNumberGenerator::randomize);

	ClassDB::bind_method(D_METHOD("randchoice", "from"), &RandomNumberGenerator::randchoice);
	ClassDB::bind_method(D_METHOD("randshuffle", "array"), &RandomNumberGenerator::randshuffle);
	ClassDB::bind_method(D_METHOD("randdecision", "probability"), &RandomNumberGenerator::randdecision);
	ClassDB::bind_method(D_METHOD("randroll", "count", "sides"), &RandomNumberGenerator::randroll);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "state"), "set_state", "get_state");
	// Default values are non-deterministic, override for doc generation purposes.
	ADD_PROPERTY_DEFAULT("seed", 0);
	ADD_PROPERTY_DEFAULT("state", 0);
}
