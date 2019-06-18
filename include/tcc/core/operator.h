#ifndef TCC_OPERATOR_H
#define TCC_OPERATOR_H

#include <vector>
#include <unordered_map>

#include "tcc/core/data.h"
#include "tcc/core/hlir.h"

namespace tcc {
namespace core {

class Operator;
class KernelContext;

class OperatorBuilder {
    public:
        OperatorBuilder(const std::string type_name);

        OperatorBuilder& Attr(const std::string attr_name, const Datatype datatype);
        OperatorBuilder& Input(const std::string input_name);
        OperatorBuilder& Output(const std::string output_name);
        OperatorBuilder& Kernel(void(*kernel)(KernelContext&));

    private:
        const std::string type_name_;
        std::unordered_map<std::string, Datatype> attr_type_map_;
        std::vector<std::string> input_list_;
        std::vector<std::string> output_list_;
        void(*kernel_)(KernelContext&) = nullptr;

    friend class Operator;
};

class Operator {
    public:
        Operator(OperatorBuilder builder);

    private:
        const std::string type_name_;
        const std::unordered_map<std::string, Datatype> attr_type_map_;
        const std::vector<std::string> input_list_;
        const std::vector<std::string> output_list_;

    public:
        static bool Exists(std::string type_name);
        static Operator Create(std::string type_name);

    private:
        static std::shared_ptr<std::unordered_map<std::string, Operator>> registry_;

    friend class HLIR::Operation;
};

} // namespace core
} // namespace tcc

#define REGISTER_OP(type_name) \
    REGISTER_OP_UNIQ_HELPER(__COUNTER__, type_name)
#define REGISTER_OP_UNIQ_HELPER(ctr, type_name) \
    REGISTER_OP_UNIQ(ctr, type_name)
#define REGISTER_OP_UNIQ(ctr, type_name) \
    static ::tcc::core::Operator operator_##ctr = \
        ::tcc::core::OperatorBuilder(type_name)

#endif // TCC_OPERATOR_H
