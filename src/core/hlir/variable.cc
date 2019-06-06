#include "tcc/core/hlir/variable.h"

#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace hlir {

Variable::Variable(const std::string instance_name, const common::Datatype datatype) :
    instance_name_(instance_name),
    datatype_(datatype) {
}

Variable::Variable(const std::string instance_name, common::Data data) :
    instance_name_(instance_name),
    datatype_(data.GetType()),
    data_(std::make_shared<common::Data>(data)) {
}

} // namespace hlir
} // namespace core
} // namespace tcc
