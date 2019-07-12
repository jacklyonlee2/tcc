#ifndef TCC_LLIR_VISUALIZE_H
#define TCC_LLIR_VISUALIZE_H

#include <sstream>
#include <unordered_map>

#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* LLIRVisualizer convert the IR to a DOT graph
 * and write to the output stream. */
class LLIRVisualizer : public LLIRVisitor {
    public:
        /* Write DOT syntax to specified output path. */
        void write(std::string output_path);

    protected:
        void add_node(Expr, std::string);
        void add_edge(Expr, Expr);

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

        std::unordered_map<Expr, std::string> node_name_map;
        std::stringstream stream;
};

} // namespace core
} // namespace tcc

#endif // TCC_LLIR_VISUALIZE_H
