#ifndef TCC_CORE_IR_PRINTER_H
#define TCC_CORE_IR_PRINTER_H

#include "tcc/core/ir_visitor.h"
#include <fstream>
#include <unordered_map>

namespace tcc {

/* ir_printer generate graphviz dot file
 * corresponding to the given ir */
struct ir_printer : ir_visitor
{
  public:
    static void apply(const std::string, expr);

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
    void visit(reduce_expr) override;
    void visit(unary_expr) override;
    void visit(binary_expr) override;

    std::ofstream file;
};

} // namespace tcc

#endif // TCC_CORE_IR_PRINTER_H
