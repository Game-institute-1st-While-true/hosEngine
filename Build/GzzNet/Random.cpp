#include "Random.h"

gzz::Random::Random()
    : generator(rd())
{
}

gzz::Random::~Random()
{
}

int gzz::Random::GetRand(const int src, const int dst)
{
	std::uniform_int_distribution<int> dis(src, dst);
	return dis(generator);
}

double gzz::Random::GetRand(const double src, const double dst, int precision)
{
	std::uniform_int_distribution<int> dis(static_cast<int>(src * precision), static_cast<int>(dst * precision));
	int ret = dis(generator);

	return ret / static_cast<double>(precision);
}
