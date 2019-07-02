#ifndef TCC_LLIR_H
#define TCC_LLIR_H

#include "tcc/core/llir/primitive.h"

namespace tcc {
namespace core {

class LLIRVisitor {
    template<typename T> friend struct Expression;
    template<typename T> friend struct Primitive;
};

class LLIR {
    public:
        LLIR(std::unordered_set<Pmt> terminal_pmts_) :
            terminal_pmts(terminal_pmts_) {}

        void accept(LLIRVisitor *v) const;

    private:
        std::unordered_set<Pmt> terminal_pmts;
};

} // namespace core
} // namespace tcc

#endif // TCC_LLIR_IR_H
