#include <iostream>
#include <algorithm>
#include <cstring>
#include <math.h>
#include <random>

#include <chrono>
using namespace std::chrono;

#include "graph.h"

hamiltonianCycle graph::opt = hamiltonianCycle();
std::mutex graph::mu;
volatile bool graph::work = true;

graph::graph() :
    name(""),
    type(""),
    comment(""),
    dimension(0),
    weight_type(WEIGHT_NONE)
{
}

graph::graph(std::string name,
      std::string type,
      std::string comment,
      int dimension,
      int weight_type) :
    name(name),
    type(type),
    comment(comment),
    dimension(dimension),
    weight_type(weight_type)
{
}

hamiltonianCycle graph::getOpt()
{
    mu.lock();
    hamiltonianCycle res = opt;
    mu.unlock();
    return res;
}

unsigned int graph::getLen()
{
    mu.lock();
    unsigned int res = opt.l;
    mu.unlock();
    return res;
}

graph::~graph()
{
}

void graph::setName(std::string name)
{
    this->name = name;
}

void graph::setType(std::string type)
{
    this->type = type;
}

void graph::setComment(std::string comment)
{
    this->comment = comment;
}

void graph::setDimension(int dimension)
{
    this->dimension = dimension;
}

void graph::setWeightType(int weight_type)
{
    this->weight_type = weight_type;
}

std::string graph::getName() const
{
    return name;
}

std::string graph::getType() const
{
    return type;
}

std::string graph::getComment() const
{
    return comment;
}

int graph::getDimension() const
{
    return dimension;
}

int graph::getWeightType() const
{
    return weight_type;
}

void graph::calculate()
{

    int test[30000], I = 0;

    auto start = steady_clock::now();

    while (work)
    {
        std::vector<bool> exists(v.size());

        for (int i = 0; i < v.size(); i++)
        {
            if (exists[i])
            {
                continue;
            }

            unsigned int minL = UINT32_MAX;
            unsigned int curL = UINT32_MAX;
            int minJ;
            int curJ;

            for (int j = i + 1; j < v.size(); j++)
            {
                if (exists[j])
                {
                    continue;
                }

                unsigned int l = TO_EUC_2D(v[i], v[j]);
                if (l < minL)
                {
                    minJ = j;
                    minL = l;
                }
            }

            hamiltonianCycle c;
            c.v.push_back(v[i]);
            exists[i] = true;

            if (minL != UINT32_MAX)
            {
                c.v.push_back(v[minJ]);
                c.l = minL;
                exists[minJ] = true;
                curL = minL;
                do
                {
                    curJ = 0;

                    for (int j = 1; j < v.size(); j++)
                    {
                        if (exists[j])
                        {
                            continue;
                        }

                        unsigned int l = TO_EUC_2D(v[minJ], v[j]);
                        if (l <= curL)
                        {
                            curJ = j;
                            curL = l;
                        }
                    }

                    if (curJ != 0)
                    {
                        exists[curJ] = true;
                        c.v.push_back(v[curJ]);
                        c.l += curL;
                        minJ = curJ;
                    }

                } while (curJ);

                c.l += TO_EUC_2D(c.v[c.v.size() - 1], c.v[0]);
            }
            else
            {
                c.l = 1;
            }

            std::vector<hamiltonianCycle>::iterator n =
                    std::upper_bound(cycles.begin(), cycles.end(), c.l,
                    [](unsigned int l, hamiltonianCycle &x) {
                        return l < x.l;
                    });

            int idx = std::distance(cycles.begin(), n);
            if (idx == cycles.size())
            {
                cycles.push_back(c);
            }
            else
            {
                cycles.resize(cycles.size() + 1);
                for (int k = cycles.size() - 1; k > idx; k--)
                {
                    cycles[k] = cycles[k - 1];
                }
                cycles[idx] = c;
            }
        }

        int off = 0;

        while (cycles.size() > 1)
        {
            if (off >= cycles.size() / 1.15)
            {
                off = 0;
            }

            hamiltonianCycle c = cycles[cycles.size() - 1 - off];
            cycles.erase(cycles.begin() + cycles.size() - 1 - off);
            hamiltonianCycle newC;
            off++;

            int minL = INT32_MAX;
            int minJ;
            int curI;
            int curK;
            unsigned char type = 0;

            if (c.v.size() == 1)
            {
                for (int j = 0; j < cycles.size(); j++)
                {
                    if (cycles[j].v.size() == 1)
                    {
                        int l = TO_EUC_2D(cycles[j].v[0], c.v[0]) * 2 - 1;

                        if (l < minL)
                        {
                            minL = l;
                            minJ = j;
                            curK = 0;
                        }
                    }
                    else
                    {
                        for (int k = 0; k < cycles[j].v.size(); k++)
                        {
                            int l = + TO_EUC_2D(cycles[j].v[k], c.v[0])
                                    + TO_EUC_2D(c.v[0], cycles[j].v[(k+1)%cycles[j].v.size()])
                                    - TO_EUC_2D(cycles[j].v[k], cycles[j].v[(k+1)%cycles[j].v.size()]);

                            if (l < minL)
                            {
                                minL = l;
                                minJ = j;
                                curK = k;
                            }
                        }
                    }
                }

                newC.l = cycles[minJ].l + minL;

                for (int i = 0; i < cycles[minJ].v.size(); i++)
                {
                    if (i == (curK + 1) % cycles[minJ].v.size())
                    {
                        newC.v.push_back(c.v[0]);
                    }
                    newC.v.push_back(cycles[minJ].v[i]);
                }
            }
            else
            {
                for (int i = 0; i < c.v.size(); i++)
                {
                    for (int j = 0; j < cycles.size(); j++)
                    {
                        if (cycles[j].v.size() == 1)
                        {
                            int l = + TO_EUC_2D(c.v[i], cycles[j].v[0])
                                    + TO_EUC_2D(cycles[j].v[0], c.v[(i+1)%c.v.size()])
                                    - TO_EUC_2D(c.v[i], c.v[(i+1)%c.v.size()]);

                            if (l < minL)
                            {
                                minL = l;
                                minJ = j;
                                curI = i;
                                type = 1;
                            }
                        }
                        else
                        {
                            for (int k = 0; k < cycles[j].v.size(); k++)
                            {
                                int l = + TO_EUC_2D(c.v[i], cycles[j].v[k])
                                    + TO_EUC_2D(c.v[(i+1)%c.v.size()], cycles[j].v[(k+1)%cycles[j].v.size()])
                                    - TO_EUC_2D(c.v[i], c.v[(i+1)%c.v.size()])
                                    - TO_EUC_2D(cycles[j].v[k], cycles[j].v[(k+1)%cycles[j].v.size()]);
                                if (l < minL)
                                {
                                    minL = l;
                                    minJ = j;
                                    curI = i;
                                    curK = k;
                                    type = 2;
                                }

                                l = + TO_EUC_2D(c.v[i], cycles[j].v[(k+1)%cycles[j].v.size()])
                                    + TO_EUC_2D(c.v[(i+1)%c.v.size()], cycles[j].v[k])
                                    - TO_EUC_2D(c.v[i], c.v[(i+1)%c.v.size()])
                                    - TO_EUC_2D(cycles[j].v[k], cycles[j].v[(k+1)%cycles[j].v.size()]);
                                if (l < minL)
                                {
                                    minL = l;
                                    minJ = j;
                                    curI = i;
                                    curK = k;
                                    type = 3;
                                }
                            }
                        }
                    }

                }

                switch (type) {
                case 1:
                    for (int i = 0; i <= curI; i++)
                    {
                        newC.v.push_back(c.v[i]);
                    }
                    newC.v.push_back(cycles[minJ].v[0]);
                    for (int i = curI + 1; i < c.v.size(); i++)
                    {
                        newC.v.push_back(c.v[i]);
                    }
                    newC.l = c.l + minL;
                    break;

                case 2:
                    for (int i = 0; i <= curI; i++)
                    {
                        newC.v.push_back(c.v[i]);
                    }
                    for (int i = curK; i >= 0; i--)
                    {
                        newC.v.push_back(cycles[minJ].v[i]);
                    }
                    for (int i = cycles[minJ].v.size() - 1; i > curK; i--)
                    {
                        newC.v.push_back(cycles[minJ].v[i]);
                    }
                    for (int i = curI + 1; i < c.v.size(); i++)
                    {
                        newC.v.push_back(c.v[i]);
                    }
                    newC.l = c.l + cycles[minJ].l + minL;
                    break;

                case 3:
                    for (int i = 0; i <= curI; i++)
                    {
                        newC.v.push_back(c.v[i]);
                    }
                    for (int i = curK + 1; i < cycles[minJ].v.size(); i++)
                    {
                        newC.v.push_back(cycles[minJ].v[i]);
                    }
                    for (int i = 0; i <= curK; i++)
                    {
                        newC.v.push_back(cycles[minJ].v[i]);
                    }
                    for (int i = curI + 1; i < c.v.size(); i++)
                    {
                        newC.v.push_back(c.v[i]);
                    }
                    newC.l = c.l + cycles[minJ].l + minL;
                break;

                }
            }

            if (newC.v.size() >= 4)
            {
                _2opt(newC);
            }

            cycles.erase(cycles.begin() + minJ);
            cycles.resize(cycles.size() + 1);

            std::vector<hamiltonianCycle>::iterator n =
                    std::upper_bound(cycles.begin(), cycles.end() - 1, newC.l,
                    [](unsigned int l, hamiltonianCycle &x) {
                        return l < x.l;
                    });

            int idx = std::distance(cycles.begin(), n);
            for (int k = cycles.size() - 1; k > idx; k--)
            {
                cycles[k] = cycles[k - 1];
            }
            cycles[idx] = newC;
        }

        mu.lock();
        if (cycles[0].l < opt.l)
        {
            opt = cycles[0];
        }
        mu.unlock();

        std::random_device rd;
        std::mt19937 g(rd() + time(0));
        std::shuffle(v.begin(), v.end(), g);

        cycles.clear();
    }
}
