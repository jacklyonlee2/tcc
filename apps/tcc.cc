#include <fstream>

#include "glog/logging.h"
#include "gflags/gflags.h"
#include "tcc/parser/parser.h"

static bool ValidateFilePath(const char* flag_name, const std::string& value) {
    if (value.empty()) {
        return false;
    } if (std::fstream file = std::fstream(value, std::ios::in | std::ios::binary)) {
        return true;
    } else {
        printf("-%s: '%s' can not be opened.\n", flag_name, value.c_str());
        return false;
    }
}

DEFINE_string(file_path, "", "Path to TensorFlow frozen graph.");
DEFINE_validator(file_path, &ValidateFilePath);

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    gflags::SetUsageMessage("tcc -file_path=\"/PATH/TO/FROZEN/GRAPH\"");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // Parses TensorFlow frozen graph into TCC's HLIR
    tcc::parser::ParseFrozenGraph(FLAGS_file_path);
}
