
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctype.h>
#include <sstream>
#include <unordered_map>
#include "../include/MiniParser.hpp"

enum types
{
    _bool,
    _int,
    _char,
    _arr
};
struct variable
{
    int value;
    std::string name;
};
void create_variables(std::string &myline, std::vector<variable> &vars, MiniParser::IDMap &ids)
{
    int counter = myline.find('=');

    MiniParser mp(myline.substr(counter + 1));

    // std::cout << myline.substr(counter + 1) << std::endl;
    vars.push_back(variable{(int)mp.eval(ids), std::string(&myline[4], &myline[myline.find(".")])});
    for (variable i : vars)
    {

        ids[i.name] = i.value;
        // std::cout << std::string(&myline[4], &myline[myline.find(".")]) << std::endl;
    }
}

void func(std::ifstream &myfile, std::string &myline, std::vector<variable> &vars, MiniParser::IDMap &ids)
{
    (myline.find("//") == 0) ? (void)0 : (void)0;
    (myline.find("var") == 0) ? create_variables(myline, vars, ids) : (void)0;
    if (myline.find("int") == 0 && myline.find("print") != std::string::npos)
    {
        MiniParser mp(myline.substr(myline.find('print') + 8));

        std::cout << (int)mp.eval(ids) << std::endl;
    }
    else if (myline.find("char") == 0 && myline.find("print") != std::string::npos)
    {

        if (myline.find("[") != std::string::npos)
        {
            std::istringstream iss(std::string(&myline[myline.find("[") + 1], &myline[myline.find("]")]));

            std::string number;

            for (iss;
                 iss >> number;)
            {
                std::cout << (char)stoi(number);
            }
        }
        else
        {

            MiniParser mp(myline.substr(myline.find('print') + 2));

            std::cout << (char)mp.eval(ids);
        }
    }
    if (myline.find("String") == 0 && myline.find("print") != std::string::npos)
    {
        // MiniParser mp(myline.substr(myline.find('print') + 8));

        std::cout << std::string(&myline[myline.find("'") + 1], &myline[myline.find("'/")]) << std::endl;
    }

    if (myline.find("char") == std::string::npos && myline.find("int") == std::string::npos && myline.find("var") == std::string::npos && myline.find("=") != std::string::npos)
    {
        int point = myline.find('=');

        int counter = 0;
        for (variable &i : vars)
        {

            if (i.name == std::string(&myline[0], &myline[point - 1]))
            {

                MiniParser mp(myline.substr(point + 1));

                vars[counter].value = (int)mp.eval(ids);

                for (variable i : vars)
                {

                    ids[i.name] = i.value;
                }
            }
            counter++;
        }
    }
}
void parse(std::ifstream &myfile, std::string &myline)
{

    std::vector<variable> vars;
    MiniParser::IDMap ids;
    while (myfile)
    {

        std::getline(myfile, myline);
        if (myline.find("loop") == 0)
        {
            std::string temp = myline;
            std::getline(myfile, myline);
            for (int i = 0; i < std::stoi(temp.substr(temp.find("p") + 1)) - 1; i++)
            {
                func(myfile, myline, vars, ids);
            }
        }
        func(myfile, myline, vars, ids);
    }
    // std::cout << myline << '\n';
}

int main()
{
    std::ifstream myfile;
    myfile.open("test.geb");
    std::string myline;
    if (myfile.is_open())
    {
        parse(myfile, myline);
        // std::cout << std::endl;
    }
    else
    {
        std::cout << "Couldn't open file\n";
    }
    return 0;
}
