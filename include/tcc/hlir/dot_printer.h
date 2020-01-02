#ifndef TCC_HLIR_DOT_PRINTER_H
#define TCC_HLIR_DOT_PRINTER_H

#include "tcc/hlir/visitor.h"

namespace tcc {
namespace hlir {

/* generate a graphviz DOT file
 * corresponding to the given hlir. */
struct dot_printer : visitor_base
{
  public:
    static void apply(std::string, expr);

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

#endif // TCC_HLIR_DOT_PRINTER_H
