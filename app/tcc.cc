#include <fstream>
#include <iterator>

#include "gflags/gflags.h"
#include "tcc/core/logging.h"
#include "tcc/core/compiler.h"
#include "tcc/frontend/frontend.h"

// Utilities

static std::vector<std::string> SplitString(std::string str, std::string delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        tokens.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }
    if (!str.empty()) {
        tokens.push_back(str);
    }
    return tokens;
}

static std::unordered_map<std::string, std::vector<long>> ParseInputShapes(std::string value) {
    std::unordered_map<std::string, std::vector<long>> input_shapes;

    // Split value by empty space
    std::istringstream buffer(value);
    std::vector<std::string> segments{
        std::istream_iterator<std::string>(buffer),
            std::istream_iterator<std::string>()};

    for (std::string segment : segments) {
        // Split segment by '=' sign
        std::vector<std::string> pair = SplitString(segment, "=");
        if (pair.size() == 2 && pair[1].size() >= 2) {
            // Stripe '()' and split shape_str by ","
            std::string shape_str = pair[1].substr(1, pair[1].size()-2);
            std::vector<std::string> dims = SplitString(shape_str, ",");
            // Convert string vec to long vec
            std::vector<long> shape;
            for (std::string dim : dims) {
                try {
                    shape.push_back(std::stol(dim, nullptr));
                } catch (...) {
                    printf("String to long conversion can not be completed.");
                    return {};
                }
            }
            // Add to input_shapes
            input_shapes.insert({pair[0], shape});
        } else {
            return {};
        }
    }

    return input_shapes;
}

// Validators

static bool ValidateInputPath(const char* flag_name, const std::string& value) {
    if (value.empty()) {
        return false;
    } else if (std::fstream file = std::fstream(value, std::ios::in | std::ios::binary)) {
        return true;
    } else {
        printf("-%s: '%s' can not be opened.\n", flag_name, value.c_str());
        return false;
    }
}

static bool ValidateInputShapes(const char* flag_name, const std::string& value) {
    if (!value.empty() && ParseInputShapes(value).empty()) {
        printf("-%s: incorrect syntax.\n", flag_name);
        return false;
    } else {
        return true;
    }
}

// Flags

DEFINE_string(input_path, "", "Path to TensorFlow frozen graph.");
DEFINE_validator(input_path, &ValidateInputPath);
DEFINE_string(input_shapes, "", "Input placeholder shapes (e.g., -input_shapes=\"pld=(1,1,224) sclr=()\").");
DEFINE_validator(input_shapes, &ValidateInputShapes);

int main(int argc, char **argv) {
    // Initialize glog
    google::InitGoogleLogging(argv[0]);

    // Parse command line arguments using gflag
    gflags::SetUsageMessage("tcc -input_path=\"/PATH/TO/FROZEN/GRAPH\"");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    BUILD_COMPILER(tcc_compiler)
        .HLIRBuilder(tcc::frontend::FromTensorFlow);

    tcc_compiler
        .BuildHLIR(FLAGS_input_path, ParseInputShapes(FLAGS_input_shapes))
        .PrintHLIR("./hlir.dot")
        .BuildLLIR();
}
