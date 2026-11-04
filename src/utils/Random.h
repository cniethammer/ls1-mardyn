#ifndef RANDOM_H
#define RANDOM_H

#include <random>

/** Uniform random number generator class.
 *
 * Wraps the Mersenne Twister std::mt19937. The conversion of its output to floating point numbers is
 * done here instead of using the distributions of the standard library, as these are implementation
 * defined. Therefore, the generated numbers are the same for all compilers and standard libraries.
 */
class Random {
public:
	Random(int seed = 8624);

	/** Restart the random number sequence with the given seed. */
	void init(int seed);

	/** Produces a random number in the open interval (0, 1). */
	float rnd();

	/** get a uniformly distributed random number in [a,b] */
	float uniformRandInRange(float a, float b) { return a + rnd() * (b - a); }

	/** Returns a value identifying the current state, e.g. to check that generators on different
	 * processes are in sync. Equal states give equal values; does not advance the sequence. */
	int getIX() const;

	/** returns a gaussian distributed deviate with zero mean and a standard deviation of stdDeviation
	 the returned value is in the range +/- infinity (better: smallest, largest double number)*/
	double gaussDeviate(double stdDeviation);

private:
	std::mt19937 _engine;
	bool _hasSpareGauss{false};  //!< gaussDeviate() generates two deviates per call, the second one is stored
	double _spareGauss{0.0};     //!< normal distributed deviate with standard deviation 1 for the next call
};

#endif
