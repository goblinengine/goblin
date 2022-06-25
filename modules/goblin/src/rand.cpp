/* 
Copyright (c) 2021 Filip Anton (filipworksdev) 
Created for Goblin Engine github.com/goblinengine
Initial implementation based on https://github.com/RodZill4/godot-music

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "rand.h"
#include "core/os/os.h"
#include "modules/regex/regex.h"
#include "core/crypto/crypto.h"

#include <stdlib.h>

Rand *Rand::singleton = nullptr;

Rand::Rand() {
	randomize();
	singleton = this;
	dice_regex.compile("^(?<repeat>\\d*[x])?\\(?(?<count>\\d{1,3})?d(?<faces>F|\\%|\\d{1,3})(?<unique>U)?(?<explode>!\\d{0,3})?(?<dropkeep>(?:[dk][lh]?\\d{0,2}))?(?<mult>[x]\\d+)?(?<addsub>[+-]\\d+)?\\)?$");
}

int Rand::i(int from, int to) {
	return randi_range(from, to);
}

real_t Rand::f(real_t from, real_t to) {
	return randf_range(from, to);
}

bool Rand::decision(float probability) {
	ERR_FAIL_COND_V_MSG(probability <= 0.0f, Variant(), "Probability must be a positive value.");
	return randbase.randf() <= probability;
}

Variant Rand::bernoulli(float probability, Variant success, Variant failure) {
	ERR_FAIL_COND_V_MSG(probability <= 0.0f, Variant(), "Probability must be a positive value.");
	if (decision(probability)) return success;
	else return failure;
}

// a number the following distributions are
// based on gdstats by doxpopuli 
// https://github.com/droxpopuli/gdstats

float Rand::normal() {
	return sqrt(-2 * log(randbase.randf())) * cos(2 * PI * randbase.randf());
}

int Rand::geometric(float probability) {
	ERR_FAIL_COND_V_MSG(probability <= 0.0f, Variant(), "Probability must be a positive value.");
	float ra = log(randbase.randf());
	float under = log(1 - probability);
	return int(ceil(ra / under));
}

int Rand::binomial(float probability, int number) {
	ERR_FAIL_COND_V_MSG(probability <= 0.0f, Variant(), "Probability must be a positive value.");
	ERR_FAIL_COND_V_MSG(number <= 0, Variant(), "Number must be a positive value.");
	int count = 0;
	while(true) {
		int curr = geometric(probability);
		if (curr > number) return count;
		count += 1;
		number -= curr;
	}
}

float Rand::exponential(float lambda) {
	ERR_FAIL_COND_V_MSG(lambda <= 0.0f, Variant(), "Lambda must be a positive value.");
	return log(1 - randbase.randf()) / (-1 * lambda);
}

int Rand::poisson(float lambda) {
	ERR_FAIL_COND_V_MSG(lambda <= 0.0f, Variant(), "Lambda must be a positive value.");
	float L = exp(-1.0f * lambda);
	float probability = 1.0f;
	int k = 0;
	
	k += 1;
	probability = probability * randbase.randf();
	
	while(probability > L) {
		k += 1;
		probability = probability * randbase.randf();
	}
		
	return k - 1;
}

int Rand::pseudo(float probability) {
	ERR_FAIL_COND_V_MSG(probability <= 0.0f || probability > 1.0f, Variant(), "Probability must be a positive value up to 1.0.");
	float curr = probability;
	int trial = 0;
	while (curr < 1.0) {
		trial += 1;
		if (decision(curr)) break;
		curr += probability;
	}
		
	return trial;
}

Variant Rand::pop(const Variant &sequence) {
	switch (sequence.get_type()) {
		case Variant::STRING: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: String is passed by value.");
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: Pool*Arrays are passed by value rather than reference.");
		} break;
		case Variant::ARRAY: {
			Array arr = sequence;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");

			int idx = randi() % arr.size();
			Variant c = arr[idx];
			// Remove unordered for performance reasons.
			arr[idx] = arr.back();
			arr.pop_back();

			return c;
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = sequence;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");

			int idx = randi() % dict.size();
			Variant c = dict.get_value_at_index(idx);
			dict.erase(dict.get_key_at_index(idx));

			return c;
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Variant();
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

Array Rand::choices(const Variant &sequence, int count, const PoolIntArray &p_weights, bool is_cumulative) {
	int sum = 0;
	LocalVector<int, int> cumulative_weights;
	LocalVector<int, int> weights;
	LocalVector<int, int> indices;
	Array weighted_choices;

	if((sequence.get_type() == Variant::DICTIONARY) && p_weights.empty()){
		Dictionary dict = sequence;
		Array w = dict.values();
		for (int i = 0; i < w.size(); i++) {
			weights.push_back(w[i]);
		}
	} else {
		for (int i = 0; i < p_weights.size(); i++) {
			weights.push_back(p_weights[i]);
		}
	}

	if(!weights.empty()){
		if(is_cumulative) {
			sum = weights[weights.size() - 1];
			cumulative_weights = weights;
		} else {
			for(int i = 0; i < weights.size(); i++) {
				if(weights[i] < 0) {
					ERR_FAIL_V_MSG(Array(), "Weights must be positive integers.");
				} else {
					sum += weights[i];
					cumulative_weights.push_back(sum);
				}
			}
		}

		for(int i = 0; i < count; i++) {
			int left = 0;
			int right = weights.size();
			int random_number = randi() % sum;
			// bisect
			while (left < right)
			{
				int mid = (left + right) / 2;
				if(cumulative_weights[mid] < random_number) {
					left = mid + 1;
				} else {
					right = mid;
				}
			}
			indices.push_back(left);
		}
	}

	switch (sequence.get_type()) {
		case Variant::STRING: {
			String str = sequence;
			ERR_FAIL_COND_V_MSG(str.empty(), Variant(), "String is empty.");
			if(weights.empty()){
				for(int i = 0; i < count; i++) {
					weighted_choices.push_back(str.substr((randi() % str.length()), 1));
				}
			} else {
				ERR_FAIL_COND_V_MSG(str.length() != weights.size(), Variant(), "Size of weights does not match.");
				for(int i = 0; i < count; i++) {
					weighted_choices.push_back(str.substr(indices[i], 1));
				}
			}
			return weighted_choices;
		} break;
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
		case Variant::ARRAY: {
			Array arr = sequence;
			ERR_FAIL_COND_V_MSG(arr.empty(), Variant(), "Array is empty.");

			if(weights.empty()){
				for(int i = 0; i < count; i++) {
					weighted_choices.push_back(arr[randi() % arr.size()]);
				}
			} else {
				ERR_FAIL_COND_V_MSG(arr.size() != weights.size(), Variant(), "Size of weights does not match.");
				for(int i = 0; i < count; i++) {
					weighted_choices.push_back(arr[indices[i]]);
				}
			}
			return weighted_choices;
		} break;
		case Variant::DICTIONARY: {
			Dictionary dict = sequence;
			ERR_FAIL_COND_V_MSG(dict.empty(), Variant(), "Dictionary is empty.");
			for(int i = 0; i < count; i++) {
				weighted_choices.push_back(dict.get_key_at_index(indices[i]));
			}
			return weighted_choices;
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be indexable.");
		}
	}
	return Array();
}

void Rand::shuffle(Array array) {
	if (array.size() < 2) {
		return;
	}
	for (int i = array.size() - 1; i > 0; --i) {
		const uint32_t j = rand() % (i + 1);
		const Variant tmp = array[i];
		array[i] = array[j];
		array[j] = tmp;
	}
}

Variant Rand::roll(uint32_t count, uint32_t faces) {
	Dictionary roll_res;	

	// silently fail if faces or rolls are too small or too big
	if (faces < 2 || faces > 144 || count < 1 || count > 100) {
		return roll_res;
	}

	Array rolls = *new Array();
	int val;
	int sum = 0;
	
	for (int c = 0; c < count; c++) {
		val = randi_range(1, faces);
		rolls.append(val);
		sum += val;
	}

	roll_res["sum"] = sum;
	roll_res["rolls"] = rolls;

	return roll_res;
}

/*
Rx ( AdX  U !E {kdlmMhY} xC +-B )

Rx: repeat everything R times (max 100 times)
AdX: roll A dice with X faces (max 100 dice with 144 faces)
U: unique rolls if possible (exploded dice don't have to be unique)
!E: explode (reroll and add to sum) when E is rolled (explosion happens after unique but before keeping/dropping and exploded rolls go in their own array)
kdlhY: (k)eep/(d)drop (l)ow/(h)igh Y number of dice (can only be used once after unique and exploded rolls don't get dropped)
xC: multiply sum by C
+-B: add or subtract B from sum

mM have not yet been implemented

Example:
2d6!1 = roll 2 dice with 6 faces and explode any time you roll 1
5x(3d10+1) = roll 3 dice with 10 faces add 1 to the total sum then repeat everything 5 times and sum it all together
4d8dl1dh2 = roll 4 dice with 8 faces discard 1 lowest and 2 highest rolls
*/
Variant Rand::roll_notation(const String dice_notation) {
	Ref<RegExMatch> regex_res = dice_regex.search(dice_notation);

	Dictionary roll_res;

	if (regex_res == nullptr) {
		return roll_res;
	}

	String count_s = regex_res->get_string("count");
	String faces_s = regex_res->get_string("faces");
	String repeat_s = regex_res->get_string("repeat");
	String dropkeep_s = regex_res->get_string("dropkeep");
	String unique_s = regex_res->get_string("unique");
	String explode_s = regex_res->get_string("explode");
	String mult_s = regex_res->get_string("mult");
	String addsub_s = regex_res->get_string("addsub");

	int count = count_s.empty() ? 1 : count_s.to_int();
	bool fudge = faces_s == "F";
	int faces = faces_s == "%" ? 100 : fudge ? 3 : faces_s.to_int();

	// silently fail if faces or rolls are too small or too big
	if (faces < 2 || faces > 144 || count < 1 || count > 100) {
		return roll_res;
	}

	bool unique = !unique_s.empty();
	bool dropkeep = !dropkeep_s.empty();
	int repeat = repeat_s.empty() ? 1 : repeat_s.substr(0,repeat_s.size()-1).to_int();
	int explode = explode_s.empty() ? 0 : explode_s == "!" ? 1 : explode_s.substr(1).to_int();
	int mult = mult_s.empty() ? 1 : mult_s.substr(1).to_int();
	int addsub = addsub_s.empty() ? 0 : addsub_s.to_int();
	int total_sum = 0;
	int explodes;

	Array rolls = *new Array();
	Array exploded_rolls = *new Array();
	Array dropped_rolls = *new Array();
	int val;
	int sum;

	if (repeat > 100) repeat = 100;

	for(int r = 0; r < repeat; r++) {
		sum = 0;
		for (int c = 0; c < count; c++) {
			val = randi_range(1, faces);
			if (fudge) {
				val -= 2;
				rolls.append(val);
				sum += val;
				continue;
			} else if(!unique || (unique && rolls.size() < faces && !rolls.has(val))) {
				rolls.append(val);
				sum += val;
			} else {
				val = 0; //drop the value so it doesn't explode
			}
			if (explode > 0 && val > 0) {
				while(val == explode) {
					val = randi_range(1, faces);
					exploded_rolls.append(val);
					explodes++;
					sum += val;
				}
			}
		}
		total_sum += sum * mult + addsub;
	}

	//dropkeep
	if (dropkeep) {
		rolls.sort(); //sort the rolls
		int d_size = dropkeep_s.length();
		int dropkeep;

		if (d_size > 2) dropkeep = dropkeep_s.substr(2).to_int();
		else if (d_size > 1) dropkeep = dropkeep_s.substr(1).to_int();

		if (dropkeep > rolls.size()) dropkeep = rolls.size();
		if (dropkeep == 0) dropkeep = 1;
		if(dropkeep_s.begins_with("d")) dropkeep = rolls.size() - dropkeep;

		// TODO: implement mM
		if (dropkeep_s.begins_with("kl") || dropkeep_s.begins_with("dh")) {
			//keep lowest/drop highest
			int dropkeep_index;
			int x;
			while(rolls.size() > dropkeep) {
				dropkeep_index = rolls.size()-1;
				x = rolls.get(dropkeep_index);
				total_sum -= x;
				dropped_rolls.append(x);
				rolls.remove(dropkeep_index);
			}
		} else { 
			//default behavior is keep highest/drop lowest
			int x;
			while(rolls.size() > dropkeep) {
				x = rolls.get(0);
				total_sum -= x;
				dropped_rolls.append(x);
				rolls.remove(0);
			}
		}
	}

	roll_res["notation"] = dice_notation;
	if (explode > 0) {
		roll_res["explodes"] = explodes;
		roll_res["exploded_rolls"] = exploded_rolls;
	}
	if (dropkeep) {
		roll_res["dropped_rolls"] = dropped_rolls;
	}
	roll_res["sum"] = total_sum;
	roll_res["rolls"] = rolls;

	return roll_res;
}

Color Rand::color() {
	Color color;
	color.set_hsv(randf(), randf_range(0.0, 1.0), randf_range(0.0, 1.0));
	return color;
}

String Rand::uuid() {
	Ref<Crypto> crypto = Ref<Crypto>(Crypto::create());
	PoolByteArray data = crypto->generate_random_bytes(16);

	data.set(6, (data[6] & 0x0f) | 0x40);
	data.set(8, (data[8] & 0x3f) | 0x80);

	PoolByteArray::Read r = data.read();
	return String::hex_encode_buffer(&r[0], 16)
			.insert(8, "-")
			.insert(13, "-")
			.insert(18, "-")
			.insert(23, "-");
}

void Rand::_bind_methods() { 
	ClassDB::bind_method(D_METHOD("i", "from", "to"), &Rand::i, DEFVAL(0), DEFVAL(99));
	ClassDB::bind_method(D_METHOD("f", "from", "to"), &Rand::f, DEFVAL(0.0f), DEFVAL(1.0f));

	ClassDB::bind_method(D_METHOD("decision", "probability"), &Rand::decision);
	ClassDB::bind_method(D_METHOD("bernoulli", "probability", "sucess", "failure"), &Rand::bernoulli, DEFVAL(1), DEFVAL(0));
	
	ClassDB::bind_method(D_METHOD("normal"), &Rand::normal);
	ClassDB::bind_method(D_METHOD("geometric", "probability"), &Rand::geometric);
	ClassDB::bind_method(D_METHOD("binomial", "probability", "number"), &Rand::binomial);
	ClassDB::bind_method(D_METHOD("exponential", "lambda"), &Rand::exponential);
	ClassDB::bind_method(D_METHOD("poisson", "lambda"), &Rand::poisson);
	ClassDB::bind_method(D_METHOD("pseudo", "probability"), &Rand::pseudo);

	ClassDB::bind_method(D_METHOD("pop", "sequence"), &Rand::pop);
    ClassDB::bind_method(D_METHOD("choice", "sequence"), &Rand::choice);
	ClassDB::bind_method(D_METHOD("choices", "sequence", "count", "weights", "cumulative"), &Rand::choices, DEFVAL(1), DEFVAL(Variant()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("shuffle", "array"), &Rand::shuffle);

	ClassDB::bind_method(D_METHOD("roll", "count", "faces"), &Rand::roll);
	ClassDB::bind_method(D_METHOD("roll_notation", "dice_notation"), &Rand::roll_notation);
	
	ClassDB::bind_method(D_METHOD("color"), &Rand::color);
	ClassDB::bind_method(D_METHOD("uuid"), &Rand::uuid);

	ADD_PROPERTY_DEFAULT("seed", 0);
	ADD_PROPERTY_DEFAULT("state", 0);
}
