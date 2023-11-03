#include <iostream>
#include "Parser.h"

#include <fstream>


int main() {

    std::ifstream src("../test.txt");
    Parser parser(std::move(src));
    try {
        parser.parse_program();
    } catch (const SyntaxErrorException& e) {
        std::cout << e.what() << ". At line " << e.getLine() << ", pos. " << e.getPosition() << '.' << std::endl;
        return 0;
    }
    parser.transpile(std::cout);
}
