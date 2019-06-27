#include "tcc/core/llir.h"

namespace tcc {
namespace core {

LLIR::Fragment::Fragment(const std::string instance_name, Data data) :
    instance_name_(instance_name),
    data_(data) {
    CHECK(data.Initialized()) <<
        "data_ is uninitialized" JOIN_MSG(for)
        LLIR_FRAGMENT_IDENTIFIER(*this);
}

LLIR::Fragment::Fragment(
        const std::string instance_name, Data data, PrimitivePtr prev_pmt) :
    instance_name_(instance_name),
    data_(data),
    prev_pmt_(prev_pmt) {
    CHECK(data.Initialized()) <<
        "data_ is uninitialized" JOIN_MSG(for)
        LLIR_FRAGMENT_IDENTIFIER(*this);
    CHECK(prev_pmt != nullptr) <<
        "prev_pmt can not be nullptr" JOIN_MSG(for)
        LLIR_FRAGMENT_IDENTIFIER(*this);
}

std::vector<long> LLIR::Fragment::GetShape() const {
    CHECK(data_.Initialized()) <<
        "data_ is uninitialized" JOIN_MSG(for)
        LLIR_FRAGMENT_IDENTIFIER(*this);
    return data_.GetShape();
}

} // namespace core
} // namespace tcc
