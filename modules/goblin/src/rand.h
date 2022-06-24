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

#include "core/math/random_number_generator.h"
#include "modules/regex/regex.h"

class Rand : public RandomNumberGenerator {
	GDCLASS(Rand, RandomNumberGenerator);

private:
	static Rand *singleton;
	RegEx dice_regex;

protected:
	static void _bind_methods();

public:
	static Rand *get_singleton() { return singleton; }

	int i(int from = 0, int to = 99);
	real_t f(real_t from = 0.0f, real_t to = 1.0f);

	Variant pop(const Variant &sequence);
	Variant choice(const Variant &sequence);
	Array choices(const Variant &sequence, int count = 1, const PoolIntArray &p_weights = Variant(), bool p_is_cumulative =  false);
	void shuffle(Array array);
	bool decision(float probability);
	Variant roll(uint32_t count, uint32_t sides);
	Variant roll_notation(const String notation);
	Color color();
	String uuid();
	
	Rand();
	~Rand() {};
};
