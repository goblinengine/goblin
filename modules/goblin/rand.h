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
