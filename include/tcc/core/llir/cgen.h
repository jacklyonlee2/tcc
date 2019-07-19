#ifndef TCC_LLIR_LOWER_H
#define TCC_LLIR_LOWER_H

#include <sstream>
#include <unordered_map>

#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* LLIRCgen generate C code from LLIR
 * and write to the output stream. */
class LLIRCgen : public LLIRVisitor {
    public:
        /* Write generated C code to specified output path. */
        void generate(std::string output_path);

    protected:
        void add_symbol(Expr);
        std::string get_symbol(Expr);

        void visit(const expr::VarPtr) override;
        void visit(const expr::ConstPtr) override;
        void visit(const expr::RangePtr) override;
        void visit(const expr::IndexPtr) override;
        void visit(const expr::ExpPtr) override;
        void visit(const expr::SqrtPtr) override;
        void visit(const expr::AddPtr) override;
        void visit(const expr::SubPtr) override;
        void visit(const expr::MulPtr) override;
        void visit(const expr::DivPtr) override;
        void visit(const expr::ModPtr) override;
        void visit(const expr::GreaterPtr) override;
        void visit(const expr::GreaterEqualPtr) override;
        void visit(const expr::LessPtr) override;
        void visit(const expr::AndPtr) override;
        void visit(const expr::SelectPtr) override;
        void visit(const expr::ReducePtr) override;

        std::unordered_set<Expr> inputs;
        std::unordered_set<Expr> outputs;
        std::unordered_map<Expr, std::string> symbol_map;
        std::stringstream prefix, body, suffix;
};

} // core
} // tcc

#endif // TCC_LLIR_LOWER_H
