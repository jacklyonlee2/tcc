#ifndef TCC_LLIR_H
#define TCC_LLIR_H

#include <unordered_set>

#include "tcc/core/llir/primitive.h"

namespace tcc {
namespace core {

class LLIRVisitor {
    public:
        /* Check if Primitive/Expression is already visited;
         * if not, dispatch to approprate visit method. */
        virtual void recurse(PmtInput);

    protected:
        /* By default following visit methods recursively
         * traverse the input of each LLIR Primitive and
         * Expression coupled with each input.
         * Implemented by calling 'recurse' on PmtInputs. */
        virtual void visit(const pmt::PlaceholderPtr);
        virtual void visit(const pmt::ConstantPtr);

        virtual void visit(const expr::RangePtr);

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
