#pragma once

#include <random>

class Random
{
public:
	static void Init()
	{
		m_RandomEngine.seed(std::random_device()());
	}

	static int RandInt(int min, int max)
	{
		SetDistributionRange(min, max);

		return m_Distribution(m_RandomEngine);
	}

private:
	static void SetDistributionRange(int min, int max) { m_Distribution.param(param_t(min, max)); }

private:
	static std::mt19937 m_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> m_Distribution;

	using param_t = std::uniform_int_distribution<std::mt19937::result_type>::param_type;
};

