#include "Random.h"

std::mt19937 Random::m_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::m_Distribution;