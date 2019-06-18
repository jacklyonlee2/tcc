#include <fstream>

#include "gflags/gflags.h"
#include "tcc/core/logging.h"
#include "tcc/core/compiler.h"
#include "tcc/frontend/frontend.h"

static bool ValidateInputPath(const char* flag_name, const std::string& value) {
    if (value.empty()) {
        return false;
    } if (std::fstream file = std::fstream(value, std::ios::in | std::ios::binary)) {
        return true;
    } else {
        printf("-%s: '%s' can not be opened.\n", flag_name, value.c_str());
        return false;
    }
}

DEFINE_string(input_path, "", "Path to TensorFlow frozen graph.");
DEFINE_validator(input_path, &ValidateInputPath);

int main(int argc, char **argv) {
    // Initialize glog
    google::InitGoogleLogging(argv[0]);

    // Parse command line arguments using gflag
    gflags::SetUsageMessage("tcc -input_path=\"/PATH/TO/FROZEN/GRAPH\"");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    BUILD_COMPILER(tcc_compiler)
        .HLIRBuilder(tcc::frontend::FromTensorFlow);

    tcc_compiler
        .BuildHLIR(FLAGS_input_path)
        .PrintHLIR("./hlir.dot");
}
