#include "tcc/common/logging.h"
#include "tcc/core/ir_codegen.h"
#include "tcc/core/ir_printer.h"
#include "tcc/frontend/parser.h"
#include <dlfcn.h>
#include <iostream>
#include <unordered_map>
#include <vector>

struct tcc_config
{
    std::string input_path;
    std::unordered_map<std::string, tcc::dimensions> input_shapes;
    std::string func_name;
};

static void print_usage_and_exit()
{
    std::cout << "Usage: tcc -input-path=\"./example.pb\" "
                 "-input-shapes=\"{a:[1,2],b:[3,4]}\" -func-name=\"example\"\n"
              << "\t-input-path\t- Path to frozen tensorflow graph.\n"
              << "\t-input-shapes\t- A map of input placeholder names to "
                 "placeholder shapes.\n"
              << "\t-func-name\t- Function name contained within the generated "
                 "shared library (needed for `dlsym`).\n"
              << "\t-help\t\t- Displays command line options.\n";
    exit(0);
}

static void parse_input_shapes(tcc_config& cf, std::string arg)
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

        cf.input_shapes[key] = shape;
    }
}

static tcc_config parse_config(int argc, char** argv)
{
    tcc_config cf;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-help")
        {
            print_usage_and_exit();
        }
        else if (arg.rfind("-input-path", 0) == 0)
        {
            cf.input_path = arg.substr(arg.rfind("=") + 1);
        }
        else if (arg.rfind("-input-shapes", 0) == 0)
        {
            parse_input_shapes(cf, arg);
        }
        else if (arg.rfind("-func-name", 0) == 0)
        {
            cf.func_name = arg.substr(arg.rfind("=") + 1);
        }
        else
        {
            tcc_error("unknown command line argument " + arg + ".");
        }
    }

    if (cf.input_path.empty() || cf.func_name.empty())
    {
        print_usage_and_exit();
    }

    return cf;
}

int main(int argc, char** argv)
{
    tcc_info("parsing command line flags ...");
    tcc_config cf = parse_config(argc, argv);

    tcc_info("parsing tensorflow graph into ir ...");
    tcc::expr ir = tcc::parse(cf.input_path, cf.input_shapes);

    tcc_info("generating ir dot file ...");
    tcc::ir_printer::apply("./ir.dot", ir);

    tcc_info("generating c code from ir ...");
    tcc::ir_codegen::apply(cf.func_name, ir);

    {
        std::string source_path = "./" + cf.func_name + ".c",
                    obj_path = "./" + cf.func_name + ".o",
                    lib_path = "./" + cf.func_name + ".so";

        tcc_info("compiling shared library from c code ...");
        system(("gcc -c -Wall -Werror -fPIC " + source_path).c_str());
        system(("gcc -march=native -Ofast -shared -o " + lib_path + " " +
                obj_path + " -lm")
                   .c_str());

        tcc_info("validating shared library ...");
        void* shared_lib = dlopen(lib_path.c_str(), RTLD_NOW);
        tcc_assert(shared_lib, "can not open shared library at " + lib_path);
        void* func_sym = dlsym(shared_lib, cf.func_name.c_str());
        tcc_assert(func_sym, "can not find function symbol at " + cf.func_name);
    }
}
