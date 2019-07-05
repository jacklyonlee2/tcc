#ifndef TCC_LLIR_H
#define TCC_LLIR_H

#include <unordered_set>
#include <functional>

#include "tcc/core/llir/primitive.h"

namespace tcc {
namespace core {

/* compute function allows the building of complex
 * computations using LLIR Primitives with
 * non-trivial data access patterns using LLIR Expressions.
 * The user will provide the ret shape of the computation
 * and a corresponding lambda expressing the computation.
 * The function will automatically create Range Expressions
 * based on the ret shape as the input to the lambda.
 * The Range Expressions will also be saved in the ret Pmt. */
Pmt compute(
        std::vector<long> shape,
        std::function<Pmt(Axes)> lambda);

class LLIRVisitor {
    public:
        /* Check if Primitive/Expression is already visited;
         * if not, dispatch to approprate visit method. */
        virtual void recurse(Pmt);

    protected:
        /* By default following visit methods recursively
         * traverse the input of each LLIR Primitive and
         * Expression coupled with each input.
         * Implemented by calling 'recurse' on PmtInputs. */
        virtual void visit(const pmt::PlaceholderPtr);
        virtual void visit(const pmt::ConstantPtr);
        virtual void visit(const pmt::MultiplyPtr);
        virtual void visit(const pmt::SelectPtr);

        virtual void visit(const expr::ScalarPtr);
        virtual void visit(const expr::RangePtr);
        virtual void visit(const expr::AddPtr);
        virtual void visit(const expr::SubPtr);
        virtual void visit(const expr::MulPtr);
        virtual void visit(const expr::DivPtr);
        virtual void visit(const expr::GreaterEqualPtr);
        virtual void visit(const expr::LessPtr);
        virtual void visit(const expr::AndPtr);

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
