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
#include <map>
#include <set>
#include <algorithm>

static std::set<std::string> modules;
static std::map<const char*, std::vector<const char*>> depends;

/**
 * Find internal char* base on external one
 */
const char* getModule(const char* name)
{
    auto it = modules.find(name);
    if (it == modules.end())
    {
        it = modules.insert(modules.begin(), name);
    }
    return (*it).c_str();
}

std::vector<const char*>& getModuleDependencies(const char* name)
{
    return depends[getModule(name)];
}

bool DependsOn(const char* name,const char* depends_on)
{
    if (depends_on != nullptr)
    {
        auto &v = depends[name];
        for (auto d : v)
        {
            if (d == depends_on)
                return true;
            if (DependsOn(d, depends_on))
                return true;
        }
    }
    return false;
}

/**
 * List of modules initialize at early level < 7
 */
static const char* const subsys[] = {
        "bluetooth.ko", /* bt_init */
        "snd.ko" ,  /* alsa_sound_init */
        "hwmon.ko", /* hwmon_init */
        "wmi.ko",
        "mmc_core.ko",
        "videodev.ko",
        "rc-core.ko"   /* rc-main.c */
};

static const char* const symbols[] = {
        "soundcore.ko", /* sound/sound_core.c */
        "libata.ko" ,
        "libahci.ko",
        "ahci.ko",
        "speedstep-lib.ko",
        "v4l2-common.ko",
        "videobuf2-memops.ko",
        "videobuf2-vmalloc.ko",
        "videobuf2-core.ko",
        "i2c-mux.ko"
};

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
    // Full dependences list to be build



    std::string line;
    std::ifstream fs(argv[1]);
    std::cout << R"(digraph {
    concentrate=true;
    rankdir=TB;
    node [fontsize=10];
    node [nodesep=0.75];
    node [ranksep=0.75];
#    edge [weight=1.2];
    node [color=none];
    node [shape=box];
#    graph [bb="0,0,1000,1000"];
#graph[size="100,100"]; 
#graph [ratio=0.5];

    subgraph cluster_0 {
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
            auto& v = getModuleDependencies(getModule(*keys.begin()));
            for (auto it2 = keys.begin() + 1; it2 != keys.end(); ++it2)
            {
                std::cout << "\"" << *it2 << "\" -> \"" << keys.front() << "\"" << std::endl;
                v.push_back(getModule(*it2));
            }
        }
    }
    // Build symbols list
   for(const char* const* ptr = symbols;ptr != symbols + sizeof(symbols)/sizeof(*symbols);++ptr)
   {
       std::cout << "\"" << *ptr << "\" [color=blue];" << std::endl;
   }
   // Build subsystem box as green
   for(const char* const* ptr = subsys;ptr != subsys + sizeof(subsys)/sizeof(*subsys);++ptr)
   {
       std::cout << "\"" << *ptr << "\" [color=green];" << std::endl;
   }
    std::cout << R"(}
   subgraph cluster_1 { 
)";
    // For each node check it its direct dependencies are found also in another path
    for (auto& mod_it : depends)
    {
        // check only multiple dependencies module
        if (mod_it.second.size() > 1)
        {
            // for each direct dependency
            for (auto& it1 : mod_it.second)
            {
                // check if it1 is included from it2
                for (auto it2 : mod_it.second)
                {
                    if ((it1 != it2) && DependsOn(it2, it1))
                    {
                        it1 = nullptr;
                        break;
                    }
                }
            }
            // remove all nullptr generated
            auto it = std::remove(mod_it.second.begin(), mod_it.second.end(),nullptr);
            mod_it.second.erase(it,mod_it.second.end());
        }
        // print out
        for (auto& it1 : mod_it.second)
        {
            std::cout << "\"" << it1 << ".2\" -> \"" << mod_it.first << ".2\"" << std::endl;
        }
    }
    // Build symbols list
    for(const char* const* ptr = symbols;ptr != symbols + sizeof(symbols)/sizeof(*symbols);++ptr)
    {
        std::cout << "\"" << *ptr << ".2\" [color=blue];" << std::endl;
    }
    // Build subsystem box as green
    for(const char* const* ptr = subsys;ptr != subsys + sizeof(subsys)/sizeof(*subsys);++ptr)
    {
        std::cout << "\"" << *ptr << ".2\" [color=green];" << std::endl;
    }
    std::cout << R"(
}
     })";
    fs.close();
    return 0;
}
