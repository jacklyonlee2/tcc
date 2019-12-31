#ifndef TCC_HLIR_VISITOR_H
#define TCC_HLIR_VISITOR_H

#include "tcc/hlir/ir.h"
#include <unordered_set>

namespace tcc {
namespace hlir {

class visitor
{
  public:
    virtual void recurse(expr);

  protected:
    virtual void visit(var_expr);
    virtual void visit(cnst_expr);
    virtual void visit(range_expr);
    virtual void visit(index_expr);
    virtual void visit(select_expr);
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
    friend struct expr_template;
};

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_VISITOR_H
