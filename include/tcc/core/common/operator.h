#ifndef TCC_OPERATOR_H
#define TCC_OPERATOR_H

#include "tcc/core/common/operator_builder.h"
#include "tcc/core/common/data.h"

namespace tcc {
namespace core {
namespace hlir { class Operation; }
namespace common {

class Operator {
    public:
        Operator(OperatorBuilder builder);

        void SetAttr(std::string attr_name, Data attr_val);

    private:
		const std::string type_name_;
        const std::unordered_map<std::string, Datatype> attr_type_map_;
        const std::vector<std::string> input_list_;
        const std::vector<std::string> output_list_;

        std::unordered_map<std::string, DataPtr> attr_val_map_;

    friend class hlir::Operation;
};

} // namespace common
} // namespace core
} // namespace tcc

#endif // TCC_OPERATOR_H
