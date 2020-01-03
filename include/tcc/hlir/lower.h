#ifndef TCC_HLIR_LOWER_H
#define TCC_HLIR_LOWER_H

#include "tcc/hlir/visitor.h"
#include "tcc/llir/ir.h"

namespace tcc {
namespace hlir {

/* lower given hlir into corresponding llir. */
struct lower : visitor_base
{
  public:
    static llir::blk apply(expr);

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
};

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_LOWER_H
