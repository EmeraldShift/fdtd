#pragma once

#include "grid.hpp"

constexpr unsigned long FLAG_VTL = 1;
constexpr unsigned long FLAG_DYN = 2;
constexpr unsigned long FLAG_PRT = 4;

struct Configuration {
	unsigned long args[4]{};
	unsigned long flags = 0;
	unsigned long threads = 0;
};

int fdtd(const Configuration &cfg);
