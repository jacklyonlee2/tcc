#ifndef TCC_CORE_IR_VISITOR_H
#define TCC_CORE_IR_VISITOR_H

#include "tcc/core/ir.h"
#include <unordered_set>

namespace tcc {

/* base class for all ir visitors. */
struct ir_visitor : std::enable_shared_from_this<ir_visitor>
{
  protected:
    virtual void visit(expr);
    virtual void visit(exprs);

    virtual void visit(var_expr);
    virtual void visit(cnst_expr);
    virtual void visit(range_expr);
    virtual void visit(index_expr);
    virtual void visit(select_expr);
    virtual void visit(reshape_expr);
    virtual void visit(exp_expr);
    virtual void visit(sqrt_expr);
    virtual void visit(add_expr);
    virtual void visit(sub_expr);
    virtual void visit(mul_expr);
    virtual void visit(div_expr);
    virtual void visit(mod_expr);
    virtual void visit(greater_expr);
    virtual void visit(greater_equal_expr);
    virtual void visit(less_expr);
    virtual void visit(logical_and_expr);
    virtual void visit(reduce_expr);

    std::unordered_set<expr> visited;

    template<typename T>
    friend struct base_expr;
};

} // namespace tcc

#endif // TCC_CORE_IR_VISITOR_H
