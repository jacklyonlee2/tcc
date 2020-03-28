#include "tcc/common/logging.h"
#include "tcc/core/ir_codegen.h"
#include "tcc/core/ir_printer.h"
#include "tcc/frontend/op.h"
#include <chrono>
#include <dlfcn.h>
#include <iostream>
#include <sys/stat.h>

static tcc::expr util_generate_cnst(tcc::dimensions shape)
{
    tcc::dimension size = std::accumulate(
        shape.begin(), shape.end(), 1, std::multiplies<tcc::dimension>());
    return tcc::cnst::make(std::vector<float>(size, 1.f), shape);
}

static void* util_compile_expr(std::string target_name, tcc::expr e)
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

    {
        tcc::ir_printer::apply(target_name, e);
        tcc_info("successfully generated dot file.");

        tcc::ir_codegen::apply(target_name, e);
        tcc_info("successfully generated source files.");
    }

    {
        const std::string gcc_compile_cmd =
            "gcc -c -fPIC -Wall -Werror -Xpreprocessor -fopenmp " +
            target_name + ".c";
        const std::string gcc_link_cmd =
            "gcc -march=native -Ofast -shared -o " + target_name + ".so " +
            target_name + ".o -lm -lomp";

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

static float* util_zero_array(int size)
{
    return (float*)calloc(size, sizeof(float));
}

static void test_conv2d(std::string target_name)
{
    tcc::expr input = util_generate_cnst({ 1, 7, 7, 1 });
    tcc::expr filter = util_generate_cnst({ 3, 3, 1, 1 });
    tcc::expr bias = util_generate_cnst({ 1 });
    tcc::expr output = build_conv2d(
        "NHWC", "SAME", { 1, 1, 1, 1 }, { 1, 1, 1, 1 }, input, filter);
    output = build_biasadd("NHWC", output, bias);

    void (*conv2d)(float*) =
        (void (*)(float*))util_compile_expr(target_name, output);

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    float* out = util_zero_array(7 * 7);

    begin = std::chrono::steady_clock::now();
    conv2d(out);
    end = std::chrono::steady_clock::now();

    tcc_info(
        "inference time: " +
        std::to_string(
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                .count()) +
        " us.");

    for (int i = 0; i < 7 * 7; i++)
        std::cout << out[i] << " ";
    std::cout << std::endl;

    free(out);
}

#define TEST(target_name)                                                      \
    tcc_info("starting " #target_name " test.");                               \
    test_##target_name(#target_name);                                          \
    tcc_info("completed " #target_name " test.")

int main()
{
    TEST(conv2d);
}

#undef TEST
