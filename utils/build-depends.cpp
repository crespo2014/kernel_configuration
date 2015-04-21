/*
 * build-depends.cpp
 *
 * Build dependence tree for dot base on module.dep
 *
 *  Created on: 21 Apr 2015
 *      Author: lester
 *  g++ -std=c++11 -g
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
    std::string line;
    std::ifstream fs(argv[1]);
    for (;;)
    {
        fs.getline(cline,sizeof(cline),'\n');
        if (fs.bad() || fs.eof() || fs.fail()) break;
        //find module name, waiting for letter

        //any / restart the pointer to next position

        //stop on .ko and go to the next

        //std::cout << cline << std::endl;
    }
    fs.close();
    return 0;
}
