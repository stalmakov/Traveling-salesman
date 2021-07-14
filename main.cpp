#include <iostream>
#include <algorithm>
#include <fstream>
#include <regex>
#include <thread>
#include <chrono>
#include <signal.h>
#include "graph.h"

using namespace std;

bool headerParsing(ifstream & file, graph & mainGraph);
bool coordSecParsing(ifstream & file, graph & mainGraph);
void CtrCHandler(int s);
void start(graph & g);

int main(int argc, char * argv[])
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = CtrCHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    if (argc == 1)
    {
        cout << "Use:\n\t" << argv[0] << " <infile>.tsp\n";
        return 0;
    }

    graph mainGraph;
    ifstream file(argv[1]);

    if (!file)
    {
        cout << "Can't open file\n";
        return 1;
    }

    if (!(headerParsing(file, mainGraph) &&
          coordSecParsing(file, mainGraph)))
    {
        cout << "Parsing error\n";
        return 1;
    }

    file.close();

    const auto processorCount = thread::hardware_concurrency();
    vector<graph> g;

    for (int i = 0; i < processorCount; i++)
    {
        g.push_back(mainGraph);
    }

    for (int i = 0; i < processorCount; i++)
    {
        thread t(start, ref(g[i]));
        t.detach();
    }

    while (true)
    {
        unsigned int len = graph::getLen();
        if (len == UINT32_MAX)
        {
            cout<<"\nCurrent len = calculating...\n";
        }
        else
        {
            cout<<"\nCurrent len = "<<len<<endl;
        }
        cout<<"Type Ctrl+C to stop.\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    return 0;
}

bool headerParsing(ifstream & file, graph & mainGraph)
{
    string buffer;
    string name;
    string value;

    while (getline(file, buffer))
    {
        if (buffer.find("NODE_COORD_SECTION") != string::npos)
        {
            return true;
        }

        size_t startpos = buffer.find_first_of(":");

        if (startpos != string::npos)
        {
            name = buffer.substr(0, startpos);
            value = buffer.substr(startpos);

            startpos = value.find_first_not_of(": \t");

            if (startpos != string::npos)
            {
                value = value.substr(startpos);

                if (value[value.size() - 1] == '\r')
                {
                    value = value.substr(0, value.size() - 1);
                }
            }
            else
            {
                continue;
            }

            transform(name.begin(), name.end(), name.begin(), ::toupper);

            if (name.find("NAME") != string::npos)
            {
                mainGraph.setName(value);
            }
            else if (name.find("EDGE_WEIGHT_TYPE") != string::npos)
            {
                transform(value.begin(), value.end(), value.begin(), ::toupper);

                if (value.find("EUC_2D") != string::npos)
                {
                    mainGraph.setWeightType(WEIGHT_EUC_2D);
                }
                //else if (value.find("CEIL_2D") != string::npos)
                //{
                //    mainGraph.setWeightType(WEIGHT_CEIL_2D);
                //}
                //else if (value.find("ATT") != string::npos)
                //{
                //    mainGraph.setWeightType(WEIGHT_ATT);
                //}
                else
                {
                    cout << "Weight type not implemented\n";
                    return false;
                }
            }
            else if (name.find("TYPE") != string::npos)
            {
                mainGraph.setType(value);
            }
            else if (name.find("COMMENT") != string::npos)
            {
                cout << value <<endl;
                mainGraph.setComment(value);
            }
            else if (name.find("DIMENSION") != string::npos)
            {
                mainGraph.setDimension(stoi(value));
            }
        }
    }

    return false;
}

bool coordSecParsing(ifstream & file, graph & mainGraph)
{
    int i = 0;
    string buffer;

    mainGraph.v.reserve(mainGraph.getDimension());

    while (getline(file, buffer))
    {
        if (buffer.find("EOF") != string::npos)
        {
            break;
        }

        const auto r = regex(" |\r");
        vector<string> res {
                sregex_token_iterator(buffer.cbegin(), buffer.cend(), r, -1),
                sregex_token_iterator() };

        auto itr = res.begin();
        do
        {
            if (*itr == "")
            {
                res.erase(itr);
                itr = res.begin();
                continue;
            }
            itr++;

        } while (itr != res.end());

        if (res.size() == 3)
        {
            mainGraph.v.push_back(vertex(stoi(res.at(0)),
                                         stod(res.at(1)),
                                         stod(res.at(2))));
            ++i;
        }
    }

    return (i == mainGraph.getDimension()) ? true : false;
}

void CtrCHandler(int s)
{
    hamiltonianCycle c = graph::getOpt();
    if (c.l != UINT32_MAX)
    {
        cout<<"\nTOUR_SECTION\n";
        for (int i = 0; i < c.v.size(); i++)
        {
            cout<<c.v[i].n<<endl;
        }
        cout<<"-1\n";
        cout<<"EOF\n";
    }
    exit(0);
}

void start(graph & g)
{
    g.calculate();
}
