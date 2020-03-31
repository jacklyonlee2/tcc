#include "tcc/common/logging.h"
#include "tcc/core/ir_codegen.h"
#include "tcc/core/ir_printer.h"
#include "tcc/frontend/parser.h"
#include <chrono>
#include <dlfcn.h>
#include <iostream>
#include <random>
#include <sys/stat.h>

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
            "gcc -c -fPIC -Wall -Werror -Xpreprocessor -fopenmp -Ofast -march=native " +
            target_name + ".c";
        const std::string gcc_link_cmd =
            "gcc -shared -o " + target_name + ".so " +
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

static float* util_zero_array(unsigned size)
{
    return (float*)calloc(size, sizeof(float));
}

static float* util_random_array(unsigned size, float max = 255.0)
{
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution(0., max);

    float* array = (float*)malloc(size * sizeof(float));
    for (unsigned i = 0; i < size; i++)
    {
        array[i] = distribution(generator);
    }

    return array;
}

static void test_mobilenetv2(std::string target_name)
{
    static const std::string model_url =
        "https://storage.googleapis.com/mobilenet_v2/checkpoints/"
        "mobilenet_v2_1.4_224.tgz";
    static const std::string file_name = "mobilenet_v2_1.4_224";

    void (*model)(float*, float*) = (void (*)(
        float*, float*))util_compile_model(model_url,
                                           file_name,
                                           target_name,
                                           { { "input", { 1, 224, 224, 3 } } });

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    float* input = util_random_array(150528);
    float* output = util_zero_array(1001);

    model(input, output);

    begin = std::chrono::steady_clock::now();
    model(input, output);
    end = std::chrono::steady_clock::now();

    tcc_info(
        "inference time: " +
        std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
                .count()) +
        " ms.");

    free(input);
    free(output);
}

#define TEST(target_name)                                                      \
    tcc_info("starting " #target_name " test.");                               \
    test_##target_name(#target_name);                                          \
    tcc_info("completed " #target_name " test.")

int main()
{
    TEST(mobilenetv2);
}

#undef TEST
