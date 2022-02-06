#include "kernels.hpp"

#include <cstdlib>
#include <iostream>
#include <atomic>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

volatile bool global_started = false;

template <typename T>
Worker<T>::Worker(phys::params params, bool print) :
	params(params), iterations(params.nt), print(print), grid(params.nx, params.ny, params.nz) {

	// Random initial grid
	for (dim_t j = 0, lim = params.nx * params.ny * params.nz; j < lim; j++) {
		grid[j] = drand48();
	}
}

template <typename T>
void Worker<T>::popGrids(Grid &a, Grid &b)
{
	dim_t i = 0;

	a = Grid(params.nx, params.ny, params.nz);
	b = Grid(params.nx, params.ny, params.nz);
	while (i++ < 2) {
		Msg m;
		in->pop(m);
		(m.src ? b : a) = *m.grid;
		if (print)
			printf("%s(%lu) pop %s\n", typeid(*this).name(), iterations, m.src ? "b" : "a");
	}
}

template <typename T>
void Worker<T>::pushGrid()
{
	if (print)
		printf("%s(%lu) push A\n", typeid(*this).name(), iterations);
	outA->pushA({&grid, true});
	if (print)
		printf("%s(%lu) push B\n", typeid(*this).name(), iterations);
	outB->pushB({&grid, false});
}

template <typename T>
void Worker<T>::run()
{
	if (print)
		printf("%s = %ld\n", typeid(*this).name(), syscall(__NR_gettid));

	while (!global_started);

	if (print)
		printf("%s up\n", typeid(*this).name());

	// This thread may have started before affinity was set; a yield
	// should give us the chance to switch to the correct core.
	pthread_yield();

	while (iterations-- > 0) {
		// E-field workers should push before popping
		if (typeid(T) == typeid(E))
			pushGrid();

		Grid a, b;
		popGrids(a, b);

		// Bounds also differ by one between H and E fields
		dim_t x_min = typeid(T) == typeid(E);
		dim_t x_max = x_min + params.nx - 1;
		dim_t y_min = typeid(T) == typeid(E);
		dim_t y_max = y_min + params.ny - 1;
		dim_t z_min = typeid(T) == typeid(E);
		dim_t z_max = z_min + params.nz - 1;

		// Calculate delta at each location and in-place modify our grid
		for (dim_t x = x_min; x < x_max; x++) {
			for (dim_t y = y_min; y < y_max; y++) {
				for (dim_t z = z_min; z < z_max; z++) {
					grid.at(x, y, z) += diff(a, b, x, y, z);
				}
			}
		}

		// H-field workers should push after the work is done
		if (typeid(T) == typeid(H))
			pushGrid();
	}

//	if (print)
//		for (dim_t i = 0, lim = params.nx * params.ny * params.nz; i < lim; i++)
//			std::cout << " " << grid[i] << std::endl;
}

// Unfortunately, there's not really a cleaner way to write out all the calculations, but here's the gist:
// A field (Type, Dim) takes as input the two fields of opposite Type and differing Dim.
// It then computes a cross product over those two fields across space.

elem_t Hx::diff(const Grid &ey, const Grid &ez, dim_t x, dim_t y, dim_t z)
{
	return params.ch * ((ey.at(x,y,z+1)-ey.at(x,y,z))*params.cz - (ez.at(x,y+1,z)-ez.at(x,y,z))*params.cy);
}

elem_t Hy::diff(const Grid &ez, const Grid &ex, dim_t x, dim_t y, dim_t z)
{
	return params.ch * ((ez.at(x+1,y,z)-ez.at(x,y,z))*params.cx - (ex.at(x,y,z+1)-ex.at(x,y,z))*params.cz);
}

elem_t Hz::diff(const Grid &ex, const Grid &ey, dim_t x, dim_t y, dim_t z)
{
	return params.ch * ((ex.at(x,y+1,z)-ex.at(x,y,z))*params.cy - (ey.at(x+1,y,z)-ey.at(x,y,z))*params.cx);
}

elem_t Ex::diff(const Grid &hy, const Grid &hz, dim_t x, dim_t y, dim_t z)
{
	return params.ce * ((hy.at(x,y,z)-hy.at(x,y,z-1))*params.cz - (hz.at(x,y,z)-hz.at(x,y-1,z))*params.cy);
}

elem_t Ey::diff(const Grid &hz, const Grid &hx, dim_t x, dim_t y, dim_t z)
{
	return params.ce * ((hz.at(x,y,z)-hz.at(x-1,y,z))*params.cx - (hx.at(x,y,z)-hx.at(x,y,z-1))*params.cz);
}

elem_t Ez::diff(const Grid &hx, const Grid &hy, dim_t x, dim_t y, dim_t z)
{
	return params.ce * ((hx.at(x,y,z)-hx.at(x,y-1,z))*params.cy - (hy.at(x,y,z)-hy.at(x-1,y,z))*params.cx);
}
