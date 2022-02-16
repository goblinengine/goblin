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
	// return int(abs(sin(OS::get_singleton()->get_ticks_usec() % to / 12.531)) * to * 11.31) % (to - from) + from;
	return randi_range(from, to);
}

real_t Rand::f(real_t from, real_t to) {
	return randf_range(from, to);
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

Variant Rand::choices(const Variant &p_from, int count, const Array &p_weights) {
	switch (p_from.get_type()) {
		case Variant::POOL_BYTE_ARRAY:
		case Variant::POOL_INT_ARRAY:
		case Variant::POOL_REAL_ARRAY:
		case Variant::POOL_STRING_ARRAY:
		case Variant::POOL_VECTOR2_ARRAY:
		case Variant::POOL_VECTOR3_ARRAY:
		case Variant::POOL_COLOR_ARRAY:
		case Variant::ARRAY: {
			Array arr = p_from;
			ERR_FAIL_COND_V_MSG(arr.empty(), Array(), "Array is empty.");
			ERR_FAIL_COND_V_MSG(arr.size() != p_weights.size(), Array(), "Array and weights unequal size.");

			int weights_sum = 0;
			for (int i = 0; i < p_weights.size(); i++) {
				if (p_weights.get(i).get_type() == Variant::INT) {
					weights_sum += (int)p_weights.get(i);	
				} else {
					ERR_FAIL_V_MSG(Array(), "Weights are not integers.");
				}
			}

			Array choices = Array();
			while(choices.size() < count) {
				float remaining_distance = randf() * weights_sum;
				for (int i = 0; i < p_weights.size(); i++) {
					remaining_distance -= (int)p_weights.get(i);
					if (remaining_distance < 0) {
						choices.append(p_from.get(i));
						break;
					}
				}
			}

			return choices;
		} break;
		default: {
			ERR_FAIL_V_MSG(Variant(), "Unsupported: the type must be Array.");
		}
	}
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
	ClassDB::bind_method(D_METHOD("i", "from", "to"), &Rand::i);
	ClassDB::bind_method(D_METHOD("f", "from", "to"), &Rand::f);

    ClassDB::bind_method(D_METHOD("choice", "from"), &Rand::choice);
    ClassDB::bind_method(D_METHOD("choices", "from", "count", "weights"), &Rand::choices);
	ClassDB::bind_method(D_METHOD("shuffle", "array"), &Rand::shuffle);
	ClassDB::bind_method(D_METHOD("decision", "probability"), &Rand::decision);
	ClassDB::bind_method(D_METHOD("roll", "count", "faces"), &Rand::roll);
	ClassDB::bind_method(D_METHOD("roll_notation", "dice_notation"), &Rand::roll_notation);
	ClassDB::bind_method(D_METHOD("color"), &Rand::color);
	ClassDB::bind_method(D_METHOD("uuid"), &Rand::uuid);

	ADD_PROPERTY_DEFAULT("seed", 0);
	ADD_PROPERTY_DEFAULT("state", 0);
}
