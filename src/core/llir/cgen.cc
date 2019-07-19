#include "tcc/core/llir/cgen.h"

#include <fstream>

namespace tcc {
namespace core {

void LLIRCgen::generate(std::string output_path) {
    std::ofstream file(output_path + ".c", std::ios::trunc);
    CHECK(file) << "Failed to open file at '" << output_path << ".c'.";
    CHECK(body.str().empty()) << "body content must be flushed to prefix.";
    /* Write generated code to stream. */
    file << prefix.str() << suffix.str();
    file.close();
}

void LLIRCgen::add_symbol(Expr expr) {
    CHECK_KEY_NOT_IN_MAP(expr, symbol_map);

    /* Insert new symbol. */
    static int count = 0;
    symbol_map.insert({expr, "v_" + std::to_string(count)});
    count++;

    /* Add axis as symbol. */
    for (Expr axis : expr->axes) {
        CHECK(axis->expr_type == ExprType::Range);
        symbol_map.insert({axis, "i_" + std::to_string(count)});
        count++;
    }

    /* Record terminal inputs. */
    if (expr->expr_type == ExprType::Var) {
        inputs.insert(expr);
    }

    /* Record potiental terminal outputs. */
    outputs.insert(expr);
}

std::string LLIRCgen::get_symbol(Expr expr) {
    /* Remove from potiental terminal outputs. */
    if (outputs.find(expr) != outputs.end()) {
        outputs.erase(expr);
    }

    CHECK_KEY_IN_MAP(expr, symbol_map);
    return symbol_map.at(expr);
}

void LLIRCgen::visit(const expr::VarPtr expr) {
    add_symbol(expr);
}

void LLIRCgen::visit(const expr::ConstPtr expr) {
    add_symbol(expr);
}

void LLIRCgen::visit(const expr::RangePtr) {
    /* Symbol should be added as axes stored in each Expr. */
}

void LLIRCgen::visit(const expr::IndexPtr expr) {
    for (Expr index : expr->indices) {
        recurse(index);
    }
    recurse(expr->tensor);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::ExpPtr expr) {
    recurse(expr->x);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::SqrtPtr expr) {
    recurse(expr->x);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::AddPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::SubPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::MulPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::DivPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::ModPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::GreaterPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::GreaterEqualPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::LessPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::AndPtr expr) {
    recurse(expr->x);
    recurse(expr->y);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::SelectPtr expr) {
    recurse(expr->condition);
    recurse(expr->t);
    recurse(expr->f);

    add_symbol(expr);
}

void LLIRCgen::visit(const expr::ReducePtr expr) {
    for (Expr axis : expr->reduce_axes) {
        recurse(axis);
    }
    recurse(expr->input);

    add_symbol(expr);
}

} // namespace core
} // namespace tcc
