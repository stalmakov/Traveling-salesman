#include "hamiltoniancycle.h"

#include <algorithm>
#include <iostream>

hamiltonianCycle::hamiltonianCycle() {
    this->l = UINT32_MAX;
}

void _2opt(hamiltonianCycle & c)
{
    std::vector<vertex> & v = c.v;
    int curDist = 0;
    int curJ = 0;
    bool work = true;

    while (work)
    {
        work = false;

        curDist = + TO_EUC_2D(v[0], v[1])
                  + TO_EUC_2D(v[1], v[2])
                  + TO_EUC_2D(v[v.size() - 1], v[0])
                  - TO_EUC_2D(v[v.size() - 1], v[1])
                  - TO_EUC_2D(v[1], v[0])
                  - TO_EUC_2D(v[0], v[2]);

        if (curDist > 0)
        {
            std::swap(v[0], v[1]);
            c.l -= curDist;
        }

        for(int i = 0; i < v.size() - 2; i++)
        {
            curDist = + TO_EUC_2D(v[i], v[i+1])
                      + TO_EUC_2D(v[i+1], v[i+2])
                      + TO_EUC_2D(v[i+2], v[(i+3)%v.size()])
                      - TO_EUC_2D(v[i], v[i+2])
                      - TO_EUC_2D(v[i+2], v[i+1])
                      - TO_EUC_2D(v[i+1], v[(i+3)%v.size()]);

            if (curDist > 0)
            {
                std::swap(v[i+1], v[i+2]);
                c.l -= curDist;
                work = true;
            }

            int dist = TO_EUC_2D(v[i], v[i+1]);
            curDist = 0;

            for(int j = i + 2, end = v.size() - 1; j < end; j++)
            {
                if (dist + TO_EUC_2D(v[j], v[j+1]) - TO_EUC_2D(v[i], v[j]) - TO_EUC_2D(v[i+1], v[j+1]) > curDist)
                {
                    curDist = dist + TO_EUC_2D(v[j], v[j+1]) - TO_EUC_2D(v[i], v[j]) - TO_EUC_2D(v[i+1], v[j+1]);
                    curJ = j;
                }
            }

            if (dist + TO_EUC_2D(v[v.size() - 1], v[0]) - TO_EUC_2D(v[i], v[v.size() - 1]) - TO_EUC_2D(v[i+1], v[0]) > curDist)
            {
                curDist = dist + TO_EUC_2D(v[v.size() - 1], v[0]) - TO_EUC_2D(v[i], v[v.size() - 1]) - TO_EUC_2D(v[i+1], v[0]);
                curJ = v.size() - 1;
            }

            if (curDist > 0)
            {
                std::reverse(&v[i+1], &v[curJ + 1]);
                c.l -= curDist;
                work = true;
            }
        }
    }
}
