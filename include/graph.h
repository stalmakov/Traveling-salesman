#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <mutex>

#include "hamiltoniancycle.h"

constexpr int WEIGHT_NONE    = 0;
constexpr int WEIGHT_EUC_2D  = 1;
constexpr int WEIGHT_CEIL_2D = 2;
constexpr int WEIGHT_ATT     = 3;

class graph : public hamiltonianCycle
{

public:

    graph();

    graph(std::string name,
          std::string type,
          std::string comment,
          int dimension,
          int weight_type);

    ~graph();

    void setName(std::string name);
    void setType(std::string type);
    void setComment(std::string comment);
    void setDimension(int dimension);
    void setWeightType(int weight_type);
    std::string getName() const;
    std::string getType() const;
    std::string getComment() const;
    int getDimension() const;
    int getWeightType() const;
    void calculate();

    static hamiltonianCycle getOpt();
    static unsigned int getLen();

private:

    std::string name;
    std::string type;
    std::string comment;
    int dimension;
    int weight_type;
    std::vector<hamiltonianCycle> cycles;

    static std::mutex mu;
    static hamiltonianCycle opt;
    static volatile bool work;
};

#endif // GRAPH_H
