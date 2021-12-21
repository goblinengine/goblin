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

	int i(int from, int to);
	real_t f(real_t from, real_t to);

	Variant choice(const Variant &p_from);
	void shuffle(Array p_array);
	bool decision(float probability);
	Variant roll(uint32_t count, uint32_t sides);
	Variant roll_notation(const String notation);
	Color color();
	
	Rand();
	~Rand() {};
};
