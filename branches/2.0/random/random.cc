
#include "random.h"
#include <time.h>
#include <stdio.h>
#include <errno.h>

CRandomMersenne Random::s_cRandomMersenne(1);

bool Random::init(int mpiRank) {
	struct timespec time;
	int ret = clock_gettime(CLOCK_REALTIME, &time);
	if (ret != 0) {
		printf("Error: clock_gettime returned %d in Random::init. errno = %d", ret, errno);
		return false;
	}
	int seed = time.tv_nsec +
			(((mpiRank + 1) * time.tv_sec) * 1000000000);
	s_cRandomMersenne.RandomInit(seed);
	return true;
}

int Random::getInt(int min, int max) {
	return s_cRandomMersenne.IRandom(min, max);
}

FLOAT Random::getFloat(FLOAT min, FLOAT max) {
	return (s_cRandomMersenne.Random() * (max - min)) + min;
}
