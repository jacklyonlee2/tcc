#ifndef TCC_HLIR_H
#define TCC_HLIR_H

#include <unordered_set>

#include "tcc/core/hlir/operation.h"

namespace tcc {
namespace core {

class HLIRVisitor {
    public:
        /* Check if Operation is already visited;
         * if not, dispatch to approprate visit method. */
        virtual void recurse(Op);
        virtual void recurse(OpRef);

    protected:
        /* By default following visit methods recursively
         * traverse the inputs of each HLIR Operation.
         * Implemented by calling 'recurse' on inputs of
         * each Operation. */
        virtual void visit(const op::PlaceholderPtr);
        virtual void visit(const op::ConstantPtr);
        virtual void visit(const op::IntermediatePtr);
        virtual void visit(const op::AddPtr);
        virtual void visit(const op::AvgPoolPtr);
        virtual void visit(const op::BiasAddPtr);
        virtual void visit(const op::Conv2DPtr);
        virtual void visit(const op::DepthwiseConv2dNativePtr);
        virtual void visit(const op::FusedBatchNormPtr);
        virtual void visit(const op::Relu6Ptr);
        virtual void visit(const op::ReshapePtr);
        virtual void visit(const op::ShapePtr);
        virtual void visit(const op::SoftmaxPtr);
        virtual void visit(const op::SqueezePtr);

    private:
        /* Visited HLIR Operations. */
        std::unordered_set<Op> visited;

    template<typename T> friend struct Operation;
};

class HLIR {
    public:
        HLIR(std::unordered_set<Op> ops_) : ops(ops_) {}

        void accept(HLIRVisitor *v) const;

    private:
        std::unordered_set<Op> ops;
};

} // namespace core
} // namespace tcc

#endif // TCC_HLIR_IR_H
