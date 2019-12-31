#ifndef TCC_HLIR_VISITOR_H
#define TCC_HLIR_VISITOR_H

#include "tcc/hlir/ir.h"
#include <unordered_set>

namespace tcc {
namespace hlir {

struct visitor_base : std::enable_shared_from_this<visitor_base>
{
  public:
    virtual void visit(expr);
    virtual void visit(exprs);

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

typedef std::shared_ptr<visitor_base> visitor;

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_VISITOR_H
