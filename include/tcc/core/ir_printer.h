#ifndef TCC_CORE_IR_PRINTER_H
#define TCC_CORE_IR_PRINTER_H

#include "tcc/core/ir_visitor.h"
#include <fstream>
#include <unordered_map>

namespace tcc {
namespace core {

/* ir_printer generate graphviz dot file
 * corresponding to the given ir */
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

} // namespace core
} // namespace tcc

#endif // TCC_CORE_IR_PRINTER_H
