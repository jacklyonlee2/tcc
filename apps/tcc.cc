#include "tcc/parser/parser.h"

using namespace std;

int main(int argc, char **argv) {
    // Parse frozen graph
    if (argc != 2) {
        throw "Must have 1 command line argment.";
    }
    string file_path = argv[1];
    tcc::parser::ParseFrozenGraph(file_path);
}
