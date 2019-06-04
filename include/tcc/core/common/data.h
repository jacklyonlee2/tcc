#ifndef TCC_DATA_H
#define TCC_DATA_H

#include <vector>
#include <memory>

#include "tcc/core/common/datatype.h"

namespace tcc {
namespace core {
namespace common {

class Data {
    public:
        explicit Data(const std::string string); // Datatype::kString
        explicit Data(const float scalar_float32); // Datatype::kScalarFloat32
        explicit Data(const std::vector<int64_t> list_int64); // Datatype::kListInt64

        Datatype GetType() const;

    private:
        const Datatype datatype_;

        std::string string_;
        float scalar_float32_;
        std::vector<int64_t> list_int64_;
};

typedef std::shared_ptr<Data> DataPtr;

} // namespace common
} // namespace core
} // namespace tcc

#endif // TCC_DATA_H
