#include "glog/logging.h"
#include "tcc/parser/parser.h"

using namespace std;

int main(int argc, char **argv) {
    // Parse frozen graph
    CHECK(argc == 2) << "main accepts a single postional argument.";
    string file_path = argv[1];
    tcc::parser::ParseFrozenGraph(file_path);
}
