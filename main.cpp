#include <iostream>
#include "Parser.h"
int main()
{
    std::ifstream source;
    Parser parser(std::move(source));

    return 0;
}
