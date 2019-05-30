#include <fstream>

#include "glog/logging.h"
#include "gflags/gflags.h"
#include "tcc/parser/parser.h"
#include "tcc/core/hlir/hlir.h"

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

    // Parse TensorFlow frozen graph into HLIR
    tcc::core::hlir::HLIR hlir;
    tcc::parser::ParseFrozenGraph(hlir, FLAGS_file_path);

    // Optimize HLIR

    // Lower HLIR into LLIR

    // Optimize LLIR

    // Lower LLIR into CGEN

    // Optimize CGEN

    // Produce Output
}
