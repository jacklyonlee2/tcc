#include "tcc/core/common/data.h"

namespace tcc {
namespace core {
namespace common {

Data::Data(const std::string string) :
    datatype_(Datatype::kString),
    string_(string) {
}

Data::Data(const float scalar_float32) :
	datatype_(Datatype::kScalarFloat32),
	scalar_float32_(scalar_float32) {
}

Data::Data(const std::vector<int64_t> list_int64) :
	datatype_(Datatype::kListInt64),
	list_int64_(list_int64) {
}

Datatype Data::GetType() const {
    return datatype_;
}

} // namespace common
} // namespace core
} // namespace tcc

