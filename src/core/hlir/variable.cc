#include "tcc/core/hlir/variable.h"

#include "tcc/util/logging.h"

namespace tcc {
namespace core {
namespace hlir {

Variable::Variable(std::string instance_name, common::Datatype datatype) :
    instance_name_(instance_name),
    datatype_(datatype) {
}

Variable::Variable(std::string instance_name, common::DataPtr data) :
    instance_name_(instance_name) {
    CHECK_NOTNULL(data);
    datatype_ = data->GetType();
    data_ = data;
}

} // namespace hlir
} // namespace core
} // namespace tcc
