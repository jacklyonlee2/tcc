#include <fstream>
#include <iterator>

#include "glog/logging.h"
#include "gflags/gflags.h"
#include "tcc/frontend/frontend.h"
#include "tcc/core/hlir/visualize.h"
#include "tcc/core/hlir/lower.h"
#include "tcc/core/llir/visualize.h"

/* Commandline API helper functions. */

static std::vector<std::string> split_string(std::string str, std::string delimiter) {
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

static std::unordered_map<std::string, std::vector<long>> parse_input_shapes(std::string value) {
    std::unordered_map<std::string, std::vector<long>> input_shapes;

    // Split value by empty space
    std::istringstream buffer(value);
    std::vector<std::string> segments{
        std::istream_iterator<std::string>(buffer),
            std::istream_iterator<std::string>()};

    for (std::string segment : segments) {
        // Split segment by '=' sign
        std::vector<std::string> pair = split_string(segment, "=");
        if (pair.size() == 2 && pair[1].size() >= 2) {
            // Stripe '()' and split shape_str by ","
            std::string shape_str = pair[1].substr(1, pair[1].size()-2);
            std::vector<std::string> dims = split_string(shape_str, ",");
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

/* Commandline flag validators. */

static bool validate_input_path(const char* flag_name, const std::string& value) {
    if (value.empty()) {
        return false;
    } else if (std::fstream file = std::fstream(value, std::ios::in | std::ios::binary)) {
        return true;
    } else {
        printf("-%s: '%s' can not be opened.\n", flag_name, value.c_str());
        return false;
    }
}

static bool validate_input_shapes(const char* flag_name, const std::string& value) {
    if (!value.empty() && parse_input_shapes(value).empty()) {
        printf("-%s: incorrect syntax.\n", flag_name);
        return false;
    } else {
        return true;
    }
}

/* gflags macros */

DEFINE_string(input_path, "", "Path to TensorFlow frozen graph.");
DEFINE_validator(input_path, &validate_input_path);
DEFINE_string(input_shapes, "", "Input placeholder shapes (e.g., -input_shapes=\"a=(1,1,224) b=()\").");
DEFINE_validator(input_shapes, &validate_input_shapes);

using namespace tcc::frontend;
using namespace tcc::core;

int main(int argc, char **argv) {
    /* Initialize glog. */
    google::InitGoogleLogging(argv[0]);

    /* Parse command line arguments using gflag. */
    gflags::SetUsageMessage("tcc -input_path=\"/PATH/TO/FROZEN/GRAPH\"");
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    /* Parse TensorFlow frozen graph into HLIR. */
    HLIR hlir = parse_tensorflow(
            FLAGS_input_path,
            parse_input_shapes(FLAGS_input_shapes));

    /* Visualize HLIR. */
    HLIRVisualizer hlir_visualizer;
    hlir.accept(&hlir_visualizer);
    hlir_visualizer.write("./hlir.dot");

    /* Lower HLIR to LLIR. */
    HLIRLowerer hlir_lowerer;
    hlir.accept(&hlir_lowerer);
    LLIR llir = hlir_lowerer.lower();

    /* Visualize LLIR. */
    LLIRVisualizer llir_visualizer;
    llir.accept(&llir_visualizer);
    llir_visualizer.write("./llir.dot");
}

