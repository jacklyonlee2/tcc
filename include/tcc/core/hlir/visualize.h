#ifndef TCC_HLIR_VISUALIZE_H
#define TCC_HLIR_VISUALIZE_H

#include <sstream>
#include <unordered_set>

#include "tcc/core/hlir/hlir.h"

namespace tcc {
namespace core {

/* HLIRVisualizer convert the IR to a DOT graph
 * and write to the output stream. */
class HLIRVisualizer : public HLIRVisitor {
    public:
        /* Write DOT syntax to specified output path. */
        void write(std::string output_path);

    protected:
        void add_node(Op op, std::string label);
        void add_edge(Op src, Op dst, std::string label);

        void visit(const op::PlaceholderPtr) override;
        void visit(const op::ConstantPtr) override;
        void visit(const op::IntermediatePtr) override;
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

        std::unordered_set<Op> added;
        std::stringstream stream;
};

} // namespace core
} // namespace tcc

#endif // TCC_HLIR_VISUALIZE_H
