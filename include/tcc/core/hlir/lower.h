#ifndef TCC_HLIR_LOWER_H
#define TCC_HLIR_LOWER_H

#include <unordered_map>

#include "tcc/core/hlir/hlir.h"
#include "tcc/core/llir/llir.h"

namespace tcc {
namespace core {

/* HLIRLowerer lower HLIR into LLIR. */
class HLIRLowerer : public HLIRVisitor {
    public:
        /* Return LLIR lowered from HLIR. */
        LLIR lower();

    protected:
        Expr get_expr(Op op);
        void set_expr(Op op, Expr expr);

        void visit(const op::PlaceholderPtr) override;
        void visit(const op::ConstantPtr) override;
        void visit(const op::AddPtr) override;
        void visit(const op::AvgPoolPtr) override;
        void visit(const op::BiasAddPtr) override;
        void visit(const op::Conv2DPtr) override;
        void visit(const op::DepthwiseConv2dNativePtr) override;
        void visit(const op::FusedBatchNormPtr) override;
        void visit(const op::Relu6Ptr) override;
        void visit(const op::ReshapePtr) override;
        void visit(const op::ShapePtr) override;
        void visit(const op::SoftmaxPtr) override;
        void visit(const op::SqueezePtr) override;

        /* Stores lowered Placeholder/Constant/Intermediate Ops.
         * Used to connect LLIR between HLIR Ops. */
        std::unordered_map<Op, Expr> lowered_op_map;

        /* Stores terminal Expressions. */
        std::unordered_set<Expr> terminal_exprs;
};

} // namespace core
} // namespace tcc

#endif // TCC_HLIR_LOWER_H
