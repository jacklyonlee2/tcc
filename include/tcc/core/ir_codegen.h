#ifndef TCC_CORE_IR_CODEGEN_H
#define TCC_CORE_IR_CODEGEN_H

#include "tcc/core/ir_dep_analysis.h"
#include "tcc/core/ir_visitor.h"
#include <functional>
#include <sstream>
#include <unordered_map>

namespace tcc {

/* ir_codegen generates c code from ir. */
struct ir_codegen : ir_visitor
{
  public:
    static void apply(const std::string, expr);

  protected:
    void add_local_symbol(expr, std::string);
    std::string add_global_symbol(expr, std::string = {});
    std::string get_indices(exprs, dimensions = {}, exprs = {});
    std::string get_symbol(expr);
    std::string newline(int = 0);
    void nest(exprs,
              expr,
              std::function<std::string()> = nullptr,
              bool force_append = false);

    void visit(var_expr) override;
    void visit(cnst_expr) override;
    void visit(index_expr) override;
    void visit(select_expr) override;
    void visit(reshape_expr) override;
    void visit(reduce_expr) override;
    void visit(unary_expr) override;
    void visit(binary_expr) override;

    bool opt_parallelize, opt_locality;

    std::unordered_set<std::string> reusable_symbols;
    std::unordered_map<expr, std::string> global_symbols;
    std::unordered_map<expr, std::string> local_symbols;
    exprs local_ranges;

    ir_dep_analysis_result dep_analysis;
    expr output;

    std::string indent_offset;
    std::stringstream body;
};

} // namespace tcc

#endif // TCC_CORE_IR_CODEGEN_H
