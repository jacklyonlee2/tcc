#ifndef TCC_AFFN_IR_PRINTER_H
#define TCC_AFFN_IR_PRINTER_H

#include "tcc/affn/ir_visitor.h"
#include <fstream>
#include <unordered_map>

namespace tcc {
namespace affn {

/* ir_printer generate graphviz dot file
 * corresponding to the given affn ir */
struct ir_printer : ir_visitor
{
  public:
    static void apply(std::string, expr);

  protected:
    void print_node(expr,
                    std::string,
                    std::vector<std::pair<expr, std::string>> = {});
    void print_edge(expr, expr);

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

    std::ofstream file;
};

} // namespace affn
} // namespace tcc

#endif // TCC_AFFN_IR_PRINTER_H
