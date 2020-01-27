#ifndef TCC_CORE_IR_DEP_ANALYSIS_H
#define TCC_CORE_IR_DEP_ANALYSIS_H

#include "tcc/core/ir_visitor.h"

namespace tcc {

struct ir_dep_analysis_result
{
    std::unordered_set<expr> inputs;
    std::unordered_set<expr> reused_non_scalars;
};

struct ir_dep_analysis : ir_visitor
{
  public:
    static ir_dep_analysis_result apply(expr);

  protected:
    void analyze(expr);
    void visit(var_expr) override;
    void visit(index_expr) override;
    void visit(select_expr) override;
    void visit(reshape_expr) override;
    void visit(reduce_expr) override;
    void visit(unary_expr) override;
    void visit(binary_expr) override;

    ir_dep_analysis_result result;
};

} // namespace tcc

#endif // TCC_CORE_IR_DEP_ANALYSIS_H
