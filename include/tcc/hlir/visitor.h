#ifndef TCC_HLIR_VISITOR_H
#define TCC_HLIR_VISITOR_H

#include "tcc/hlir/ir.h"
#include <unordered_set>

namespace tcc {
namespace hlir {

class visitor
{
  public:
    virtual void recurse(expr);

  protected:
    virtual void visit(var_expr);
    virtual void visit(cnst_expr);

    std::unordered_set<expr> visited;

    template<typename T>
    friend struct expr_template;
};

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_VISITOR_H
