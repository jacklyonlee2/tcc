#include "tcc/common/data.h"
#include "tcc/common/logging.h"
#include "tcc/core/ir_codegen.h"
#include "tcc/core/ir_printer.h"
#include "tcc/frontend/parser.h"
#include <iostream>
#include <unordered_map>
#include <vector>

struct tcc_config
{
    std::string input_path;
    std::unordered_map<std::string, tcc::dimensions> input_shapes;
};

static void print_usage_and_exit()
{
    std::cout << "Usage: tcc -input-path=\"./example.pb\" "
                 "-input-shapes=\"{a:[1,2],b:[3,4]}\"\n"
              << "\t-input-path\t- Path to frozen tensorflow graph.\n"
              << "\t-input-shapes\t- A map of input placeholder names to "
                 "placeholder shapes.\n"
              << "\t-help\t\t- Displays command line options.\n";
    exit(0);
}

static void parse_input_path(tcc_config& config, std::string arg)
{
    config.input_path = arg.substr(arg.rfind("=") + 1);
}

static void parse_input_shapes(tcc_config& config, std::string arg)
{
    size_t pos;
    std::string value = arg.substr(arg.rfind("=") + 1);
    value = value.substr(1, value.size() - 2);

    std::vector<std::string> entries;
    while ((pos = value.find("],")) != std::string::npos)
    {
        entries.push_back(value.substr(0, pos + 1));
        value.erase(0, pos + 2);
    }
    entries.push_back(value);

    for (std::string entry : entries)
    {
        std::string key = entry.substr(0, entry.find(":"));
        std::string val = entry.substr(entry.rfind(":") + 1);
        val = val.substr(1, val.size() - 2);

        tcc::dimensions shape;
        while ((pos = val.find(",")) != std::string::npos)
        {
            shape.push_back(stol(val.substr(0, pos)));
            val.erase(0, pos + 1);
        }
        shape.push_back(stol(val));

        config.input_shapes[key] = shape;
    }
}

static tcc_config parse_config(int argc, char** argv)
{
    tcc_config config;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-help")
        {
            print_usage_and_exit();
        }
        else if (arg.rfind("-input-path", 0) == 0)
        {
            parse_input_path(config, arg);
        }
        else if (arg.rfind("-input-shapes", 0) == 0)
        {
            parse_input_shapes(config, arg);
        }
        else
        {
            tcc_error("unknown command line argument \"" + arg + "\".");
        }
    }

    if (config.input_path.empty())
    {
        print_usage_and_exit();
    }

    return config;
}

int main(int argc, char** argv)
{
    tcc_info("parsing command line flags ...");
    tcc_config conf = parse_config(argc, argv);

    tcc_info("parsing tensorflow graph into ir ...");
    tcc::expr ir = tcc::parse(conf.input_path, conf.input_shapes);

    tcc_info("generating ir dot file ...");
    tcc::ir_printer::apply("./core.dot", ir);

    tcc_info("generating c code from ir ...");
    tcc::ir_codegen::apply("./out.cc", ir);
}
