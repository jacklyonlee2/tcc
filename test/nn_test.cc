#include "tcc/common/logging.h"
#include "tcc/core/ir_codegen.h"
#include "tcc/core/ir_printer.h"
#include "tcc/frontend/parser.h"
#include <dlfcn.h>
#include <sys/stat.h>

static_assert(__unix__, "nn_test is only compatible with unix systems.");

static void* util_compile_model(
    const std::string model_url,
    const std::string file_name,
    const std::string target_name,
    std::unordered_map<std::string, tcc::dimensions> input_shapes)
{
    struct stat info;
    if (stat(target_name.c_str(), &info))
    {
        tcc_assert(!system(("rm -rf " + target_name).c_str()),
                   "failed to remove existing directory at " + target_name);
        tcc_assert(
            !mkdir(target_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH),
            "failed to create output directory at " + target_name);
        tcc_info("successfully created target directory at " + target_name);
    }

    const std::string tgz_path = target_name + "/" + file_name + ".tgz";
    if (stat(tgz_path.c_str(), &info))
    {
        tcc_assert(!system(("wget -P " + target_name + " " + model_url +
                            " > /dev/null 2>&1")
                               .c_str()),
                   "failed to download model at " + model_url);
        tcc_assert(!system(("tar -xvzf " + tgz_path + " -C ./" + target_name +
                            +" > /dev/null 2>&1")
                               .c_str()),
                   "failed to unpack model at " + tgz_path);
        tcc_info("successfully downloaded and unpacked " + tgz_path);
    }

    {
        const std::string input_path =
            target_name + "/" + file_name + "_frozen.pb";

        tcc::expr ir = tcc::parse(input_path, input_shapes);
        tcc_info("successfully parsed tensorflow graph into tcc ir.");

        tcc::ir_printer::apply(target_name, ir);
        tcc_info("successfully generated dot file.");

        tcc::ir_codegen::apply(target_name, ir);
        tcc_info("successfully generated source files.");
    }

    {
        const std::string gcc_compile_cmd =
            "gcc -c -fPIC -Wall -Werror " + target_name + ".c";
        const std::string gcc_link_cmd =
            "gcc -march=native -Ofast -shared -o " + target_name + ".so " +
            target_name + ".o -lm";

        tcc_assert(
            !system(("cd " + target_name + " && " + gcc_compile_cmd).c_str()),
            "failed to compile shared library.");
        tcc_assert(
            !system(("cd " + target_name + " && " + gcc_link_cmd).c_str()),
            "failed to link shared library.");
        tcc_info("successfully compiled shared library.");
    }

    {
        const std::string lib_path = target_name + "/" + target_name + ".so";

        void* shared_lib = dlopen(lib_path.c_str(), RTLD_NOW);
        tcc_assert(shared_lib, "can not open shared library at " + lib_path);

        void* func_sym = dlsym(shared_lib, target_name.c_str());
        tcc_assert(func_sym, "can not find function symbol.");
        return func_sym;
    }
}

static void test_mobilenetv2()
{
    static const std::string model_url =
        "https://storage.googleapis.com/mobilenet_v2/checkpoints/"
        "mobilenet_v2_1.4_224.tgz";
    static const std::string file_name = "mobilenet_v2_1.4_224";
    static const std::string target_name = "mobilenetv2";

    void* model = util_compile_model(
        model_url, file_name, target_name, { { "input", { 1, 224, 224, 3 } } });
}

int main()
{
    tcc_info("starting mobilenetv2 test.");
    test_mobilenetv2();
    tcc_info("completed mobilenetv2 test.");
}
