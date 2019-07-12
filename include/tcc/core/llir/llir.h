#ifndef TCC_LLIR_H
#define TCC_LLIR_H

#include <unordered_set>

#include "tcc/core/llir/expression.h"

namespace tcc {
namespace core {

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
        virtual void visit(const expr::IndexPtr);
        virtual void visit(const expr::ExpPtr);
        virtual void visit(const expr::SqrtPtr);
        virtual void visit(const expr::AddPtr);
        virtual void visit(const expr::SubPtr);
        virtual void visit(const expr::MulPtr);
        virtual void visit(const expr::DivPtr);
        virtual void visit(const expr::ModPtr);
        virtual void visit(const expr::GreaterPtr);
        virtual void visit(const expr::GreaterEqualPtr);
        virtual void visit(const expr::LessPtr);
        virtual void visit(const expr::AndPtr);
        virtual void visit(const expr::SelectPtr);
        virtual void visit(const expr::ReducePtr);

        /* Visited LLIR Expressions. */
        std::unordered_set<Expr> visited;

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
