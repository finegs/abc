#include <iostream>
#include <bar_tool.hpp>

bar_tool::bar_tool() : var(0) {}
bar_tool::~bar_tool() {}

void bar_tool::doStuff()
{
    std::cout << barFunction(var) << std::endl;
}
