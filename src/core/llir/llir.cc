#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* LLIRVisitor method implementations. */

void LLIRVisitor::recurse(Expr expr) {
    if (visited.find(expr) == visited.end()) {
        visited.insert(expr);
        expr->accept(this);
    }
}

void LLIRVisitor::visit(const expr::VarPtr) {
}

void LLIRVisitor::visit(const expr::ConstPtr) {
}

void LLIRVisitor::visit(const expr::RangePtr) {
}

void LLIRVisitor::visit(const expr::IndexPtr expr) {
    for (Expr index : expr->indices) {
        recurse(index);
    }
    recurse(expr->tensor);
}

void LLIRVisitor::visit(const expr::ExpPtr expr) {
    recurse(expr->x);
}

void LLIRVisitor::visit(const expr::SqrtPtr expr) {
    recurse(expr->x);
}

void LLIRVisitor::visit(const expr::AddPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::SubPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::MulPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::DivPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::ModPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::GreaterPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::GreaterEqualPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::LessPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::AndPtr expr) {
    recurse(expr->x);
    recurse(expr->y);
}

void LLIRVisitor::visit(const expr::SelectPtr expr) {
    recurse(expr->condition);
    recurse(expr->t);
    recurse(expr->f);
}

void LLIRVisitor::visit(const expr::ReducePtr expr) {
    for (Expr axis : expr->reduce_axes) {
        recurse(axis);
    }
    recurse(expr->input);
}

/* LLIR method implementations. */

void LLIR::accept(LLIRVisitor *v) const {
    for (Expr expr : terminal_exprs) {
        v->recurse(expr);
    }
}

} // namespace core
} // namespace tcc
