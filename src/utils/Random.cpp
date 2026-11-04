#include "Random.h"

#include <cmath>
#include <cstdint>

Random::Random(int seed) { init(seed); }

void Random::init(int seed) {
	_engine.seed(static_cast<std::uint32_t>(seed));
	_hasSpareGauss = false;
}

float Random::rnd() {
	// Use the upper 23 bits, so that (k + 0.5) / 2^23 is exactly representable as float.
	// This gives values in the open interval (0, 1), i.e. never 0 or 1.
	const std::uint32_t k = static_cast<std::uint32_t>(_engine()) >> 9;
	return (static_cast<float>(k) + 0.5f) * 0x1p-23f;
}

int Random::getIX() const {
	auto copy = _engine;
	return static_cast<int>(copy());
}

double Random::gaussDeviate(double stdDeviation) {
	/* Polar form of the Box-Muller transformation, see "Numerical Recipes in C++" by W.H. Press et al.,
	 * Cambridge University Press. It generates two normal distributed deviates; one is returned and
	 * the other one is stored for the next call. */
	if (_hasSpareGauss) {
		_hasSpareGauss = false;
		return _spareGauss * stdDeviation;
	}
	double v1, v2, rsq;
	do {
		v1 = 2.0 * rnd() - 1.0;
		v2 = 2.0 * rnd() - 1.0;
		rsq = v1 * v1 + v2 * v2;
	} while (rsq >= 1.0 || rsq == 0.0);
	const double fac = std::sqrt(-2.0 * std::log(rsq) / rsq);
	_spareGauss = v1 * fac;
	_hasSpareGauss = true;
	return v2 * fac * stdDeviation;
}
