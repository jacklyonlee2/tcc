#include <fstream>

#include "gflags/gflags.h"
#include "tcc/core/logging.h"
#include "tcc/core/compiler.h"
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
    // Initialize glog
    google::InitGoogleLogging(argv[0]);

    // Parse command line arguments using gflag
    gflags::SetUsageMessage("tcc -file_path=\"/PATH/TO/FROZEN/GRAPH\"");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    BUILD_COMPILER(tcc)
        .Parser(tcc::parser::TensorFlowParser);

    tcc.Compile(FLAGS_file_path);
}
