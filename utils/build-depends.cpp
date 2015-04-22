/*
 * build-depends.cpp
 *
 * Build dependence tree for dot base on module.dep
 *
 *  Created on: 21 Apr 2015
 *      Author: lester
 *  g++ -std=c++11 -g
 *  dot -Tpng  -o sample.png graphviz-queue.dot
 */


#include <vector>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    char    cline[1000];
    if (argc < 2)
    {
        std::cout << "Invalid number of arguments";
        return -1;
    }
    unsigned index;   // 0 reading main module.
    std::vector<char*> keys;
    char*   key;
    char*   pos;
    std::string line;
    std::ifstream fs(argv[1]);
    std::cout << R"(digraph {
    concentrate=true;
    rankdir=LR;
    node [fontsize=8];
    node [nodesep=0.75];
    node [ranksep=0.75];
    edge [weight=1.2];
    node [color=none];
    node [shape=plaintext];
#    graph [bb="0,0,1000,1000"];
#graph[size="100,100"]; 
    graph [ratio=2];

    
    )";
    for (;;)
    {
        fs.getline(cline,sizeof(cline),'\n');
        if (fs.bad() || fs.eof() || fs.fail()) break;
        //find module name, waiting for letter
        key = nullptr;
        pos = cline;
        keys.clear();
        //any / restart the pointer to next position
        for (pos=cline;*pos;++pos)
        {
            if (key == nullptr)
            {
                if (!isalpha(*pos))
                    continue;
                key = pos;
            }
            switch (*pos)
            {
            case '/':
            key = pos+1;
            break;
            case '.':
                if (pos[1]=='k' && pos[2] == 'o')
                {
                    if (pos[3] != 0)
                    {
                        pos[3] = 0;
                        ++pos;
                    }
                    ++pos;
                    ++pos;
                    keys.push_back(key);
                }
                key = nullptr;
            };
        }
        if (keys.size() > 1)
        {
            for (auto it2 = keys.begin() + 1; it2 != keys.end(); ++it2)
            {
                std::cout << "\"" << *it2 << "\" -> \"" << keys.front() << "\"" << std::endl;
            }
        }
    }
    std::cout << R"(})";
    fs.close();
    return 0;
}
