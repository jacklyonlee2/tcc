#ifndef TCC_CONTEXT_H
#define TCC_CONTEXT_H

#include "tcc/core/hlir.h"
#include "tcc/core/llir.h"

namespace tcc {
namespace core {

class KernelContext {
    public:
        KernelContext(HLIR::OperationPtr operation);

        Data GetAttr(std::string attr_name) const;
        LLIR::FragmentPtr GetInput(std::string input_name) const;
        void SetOutput(std::string output_name, LLIR::FragmentPtr fragment) const;

    private:
        HLIR::OperationPtr operation_;
        std::unordered_map<std::string, LLIR::FragmentPtr> input_fragment_map_;
        std::unordered_map<std::string, LLIR::FragmentPtr> output_fragment_map_;
};

} // namespace core
} // namespace tcc

#endif // TCC_CONTEXT_H
