#ifndef TCC_DATA_H
#define TCC_DATA_H

#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>

#include "tcc/core/datatype.h"
#include "tcc/core/logging.h"

#define DEFINE_DATATYPE(type_enum, type) \
    DEFINE_NAMED_DATATYPE(type_enum, type, scalar_##type)
#define DEFINE_NAMED_DATATYPE(type_enum, type, name) \
        public: \
            explicit Data(const type name) : \
                name##_(name), \
                datatype_(type_enum) {} \
        private: \
            type name##_; \

#define DEFINE_LIST_DATATYPE(type_enum, type) \
    DEFINE_NAMED_LIST_DATATYPE(type_enum, type, list_##type)
#define DEFINE_NAMED_LIST_DATATYPE(type_enum, type, name) \
        public: \
            explicit Data(const type* name, const int64_t length) : \
                name##_(name), \
                datatype_(type_enum), \
                data_length_(length) {} \
        private: \
            const type* name##_ = nullptr; \

#define DEFINE_TENSOR_DATATYPE(type_enum, type) \
    DEFINE_NAMED_TENSOR_DATATYPE(type_enum, type, tensor_##type)
#define DEFINE_NAMED_TENSOR_DATATYPE(type_enum, type, name) \
        public: \
            explicit Data( \
                    const type* name, \
                    const std::vector<int64_t> tensorshape) : \
                name##_(name), \
                datatype_(type_enum), \
                tensorshape_(tensorshape) { \
                    CHECK(!tensorshape.empty()) << "Tensor shape can not be empty."; \
                    CHECK(std::find(tensorshape.begin(), tensorshape.end(), 0) == tensorshape.end()) << \
                        "Tensor shape can not contain dimension of 0."; \
                    data_length_ = std::accumulate( \
                            tensorshape.begin(), \
                            tensorshape.end(), \
                            1, std::multiplies<int64_t>()); \
                } \
        private: \
            const type* name##_ = nullptr; \

namespace tcc {
namespace core {

class Data {
    DEFINE_NAMED_DATATYPE(Datatype::kString, std::string, string)
    DEFINE_DATATYPE(Datatype::kScalarFloat32, float)
    DEFINE_LIST_DATATYPE(Datatype::kListInt64, int64_t)
    DEFINE_TENSOR_DATATYPE(Datatype::kTensorFloat32, float)
    DEFINE_TENSOR_DATATYPE(Datatype::kTensorInt32, int32_t)

    public:
        Datatype GetType() const { return datatype_; }

    private:
        const Datatype datatype_;
        int64_t data_length_;
        std::vector<int64_t> tensorshape_;
};

} // namespace core
} // namespace tcc

#endif // TCC_DATA_H
