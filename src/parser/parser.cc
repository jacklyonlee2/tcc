#include "tcc/parser/parser.h"

#include <iostream>

#include "glog/logging.h"
#include "protos/graph.pb.h"

using namespace std;

void tcc::parser::ParseFrozenGraph(string path) {
    cout << "IN PARSER, path: " << path << endl;
    CHECK(7 > 3) << "OH NO";
}
