#ifndef TCC_AFFN_IR_LOWER_H
#define TCC_AFFN_IR_LOWER_H

#include "tcc/affn/ir_visitor.h"
#include "tcc/lang/ir.h"
#include <stack>

namespace tcc {
namespace affn {

/* ir_lower constructs lang ir from affn ir. */
struct ir_lower : ir_visitor
{
  public:
    static lang::prim apply(expr);

  protected:
    void visit(var_expr) override;
    void visit(cnst_expr) override;
    void visit(range_expr) override;
    void visit(index_expr) override;
    void visit(select_expr) override;
    void visit(reshape_expr) override;
    void visit(exp_expr) override;
    void visit(sqrt_expr) override;
    void visit(add_expr) override;
    void visit(sub_expr) override;
    void visit(mul_expr) override;
    void visit(div_expr) override;
    void visit(mod_expr) override;
    void visit(greater_expr) override;
    void visit(greater_equal_expr) override;
    void visit(less_expr) override;
    void visit(logical_and_expr) override;
    void visit(reduce_expr) override;

    std::stack<lang::prims> scopes;
};

} // namespace affn
} // namespace tcc

#endif // TCC_AFFN_IR_LOWER_H
