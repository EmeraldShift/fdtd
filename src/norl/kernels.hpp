#pragma once

#include "grid.hpp"
#include "phys.hpp"
#include "queues.hpp"

#include <iostream>

// Indicator types for H-field vs. E-field workers
struct H{};
struct E{};

struct Msg {
//	elem_t val;
	Grid *grid;
	bool src; // 0 = A, 1 = B
	bool valid = 0;

	bool operator!=(Msg const &o) {
		return o.valid;
	}

	bool operator==(Msg const &o) {
		return !o.valid;
	}
};

extern volatile bool global_started;

template <typename T>
class Worker {
protected:
	phys::params params;
	unsigned long iterations;
	Grid grid;
	bool print;
	Queue<Msg> *in, *outA, *outB;

public:
	Worker(phys::params params, bool silent);
	void popGrids(Grid &a, Grid &b);
	void pushGrid();

	void run();
	virtual elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z) = 0;

	void connect(Queue<Msg> *in, Queue<Msg> *outA, Queue<Msg> *outB) {
		this->in = in;
		this->outA = outA;
		this->outB = outB;
	}
};

#define WORKER_CLASS(Type, Dim) \
	class Type##Dim : public Worker<Type> { \
	public: \
		Type##Dim(phys::params params, bool print) : \
			Worker(params, print) {} \
		elem_t diff(const Grid &, const Grid &, dim_t x, dim_t y, dim_t z) final; \
	}

WORKER_CLASS(H, x);
WORKER_CLASS(H, y);
WORKER_CLASS(H, z);
WORKER_CLASS(E, x);
WORKER_CLASS(E, y);
WORKER_CLASS(E, z);

template class Worker<H>;
template class Worker<E>;
