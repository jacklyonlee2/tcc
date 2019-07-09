#ifndef TCC_LLIR_H
#define TCC_LLIR_H

#include <unordered_set>
#include <functional>

#include "tcc/core/llir/expression.h"

namespace tcc {
namespace core {

/* compute function allows the building of complex
 * computations using LLIR Expressions with non-trivial
 * data access patterns.
 * The user will provide the ret shape of the computation
 * and a corresponding lambda expressing the computation.
 * The function will automatically create Range Expressions
 * based on the ret shape as the input to the lambda. */
Expr compute(
        std::vector<long> shape,
        std::function<Expr(Axes)> lambda);

class LLIRVisitor {
    public:
        /* Check if Expression is already visited;
         * if not, dispatch to approprate visit method. */
        virtual void recurse(Expr);

    protected:
        /* By default following visit methods recursively
         * traverse the input of each LLIR Expression.
         * Implemented by calling 'recurse' on Expr inputs. */
        virtual void visit(const expr::VarPtr);
        virtual void visit(const expr::ConstPtr);
        virtual void visit(const expr::RangePtr);
        virtual void visit(const expr::AddPtr);
        virtual void visit(const expr::SubPtr);
        virtual void visit(const expr::MulPtr);
        virtual void visit(const expr::DivPtr);
        virtual void visit(const expr::GreaterEqualPtr);
        virtual void visit(const expr::LessPtr);
        virtual void visit(const expr::AndPtr);

    template<typename T> friend struct Expression;
};

class LLIR {
    public:
        LLIR(std::unordered_set<Expr> terminal_exprs_) :
            terminal_exprs(terminal_exprs_) {}

        void accept(LLIRVisitor *v) const;

    private:
        std::unordered_set<Expr> terminal_exprs;
};

} // namespace core
} // namespace tcc

#endif // TCC_LLIR_IR_H
