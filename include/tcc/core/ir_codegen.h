#ifndef TCC_CORE_IR_CODEGEN_H
#define TCC_CORE_IR_CODEGEN_H

#include "tcc/core/ir_visitor.h"
#include <fstream>
#include <functional>
#include <unordered_map>

namespace tcc {

typedef std::unordered_map<expr, std::string> symbol_map;

/* ir_flatten flattens scalar expr into strings. */
struct ir_flatten : ir_visitor
{
  public:
    static std::string apply(symbol_map, expr);

  protected:
    std::string get_symbol(expr);
    std::string get_symbols(exprs);
    void add_symbol(expr, std::string);

    void visit(index_expr) override;
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

    symbol_map symbols;
};

/* ir_codegen generates c code from ir. */
struct ir_codegen : ir_visitor
{
  public:
    static void apply(std::string, expr);

  protected:
    std::string add_variable(expr);
    void alias_variable(expr, expr);
    void nest(exprs, expr, std::function<std::string(symbol_map)>);

    void visit(var_expr) override;
    void visit(cnst_expr) override;
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

    symbol_map symbols;
    std::string source;
};

} // namespace tcc

#endif // TCC_CORE_IR_CODEGEN_H
