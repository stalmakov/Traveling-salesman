#ifndef HAMILTONIANCYCLE_H
#define HAMILTONIANCYCLE_H

#include <vector>
#include <math.h>
#include "vertex.h"

#define TO_EUC_2D(a, b) (unsigned int)(sqrt((b.x-a.x)*(b.x-a.x)+(b.y-a.y)*(b.y-a.y))+0.5)

struct hamiltonianCycle
{
    unsigned int l;
    std::vector<vertex> v;
    hamiltonianCycle();

};

void _2opt(hamiltonianCycle & c);

#endif // HAMILTONIANCYCLE_H
