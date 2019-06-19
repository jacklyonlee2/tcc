#ifndef TCC_DATA_H
#define TCC_DATA_H

#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>

#include "tcc/core/logging.h"

#define DEFINE_DATATYPE(metatype, dt_enum, type) \
    metatype(dt_enum, type)
#define SCALAR(dt_enum, type) \
    public: \
        static Data dt_enum() { \
            return Data(Datatype::dt_enum, {}); \
        } \
        static Data dt_enum(const type data) { \
            return Data(Datatype::dt_enum, {}, \
                    static_cast<const void*>(new type(data))); \
        }
#define VECTOR(dt_enum, type) \
    public: \
        static Data dt_enum(const long size) { \
            CHECK(size > 0) << \
                "Vector size must be bigger than 0."; \
            return Data(Datatype::dt_enum, {size}); \
        } \
        static Data dt_enum(const type* data, const long size) { \
            CHECK(size > 0) << \
                "Vector size must be bigger than 0."; \
            return Data(Datatype::dt_enum, {size}, \
                    static_cast<const void*>(data)); \
        }
#define TENSOR(dt_enum, type) \
    public: \
        static Data dt_enum(const std::vector<long> shape) { \
            CHECK(!shape.empty()) << \
                "Tensor shape can not be empty."; \
            CHECK_KEY_NOT_IN_VEC(0, shape) << \
                "Tensor shape can not contain dimension of 0."; \
            return Data(Datatype::dt_enum, shape); \
        } \
        static Data dt_enum(const type* data, const std::vector<long> shape) { \
            CHECK(!shape.empty()) << \
                "Tensor shape can not be empty."; \
            CHECK_KEY_NOT_IN_VEC(0, shape) << \
                "Tensor shape can not contain dimension of 0."; \
            return Data(Datatype::dt_enum, shape, \
                    static_cast<const void*>(data)); \
        } \
        static Data dt_enum(const char* data, const std::vector<long> shape) { \
            return Data::dt_enum(reinterpret_cast<type*>(strdup(data)), shape); \
        }

namespace tcc {
namespace core {

enum class Datatype {
    kUninitialized,
    kScalarSTR,
    kScalarFP32,
    kVectorI64,
    kTensorFP32,
    kTensorI32
};

class Data {
    public:
        Data() : datatype_(Datatype::kUninitialized), data_(nullptr) {}
        Data(Datatype datatype, const std::vector<long> shape) :
            datatype_(datatype), shape_(shape) {}
        Data(Datatype datatype, const std::vector<long> shape, const void* data) :
            datatype_(datatype), shape_(shape), data_(data) {}

        bool Initialized() const { return datatype_ != Datatype::kUninitialized; }
        bool Dataless() const { return data_ == nullptr; }

        Datatype GetType() const { return datatype_; }
        std::vector<long> GetShape() const { return shape_; }
        long GetSize() const { return std::accumulate(
                shape_.begin(),
                shape_.end(),
                1, std::multiplies<long>()); }

    private:
        Datatype datatype_;
        const std::vector<long> shape_;
        const void* data_;

    DEFINE_DATATYPE(SCALAR, kScalarSTR, std::string) // Data::kScalarSTR
    DEFINE_DATATYPE(SCALAR, kScalarFP32, float) // Data::kScalarFP32
    DEFINE_DATATYPE(VECTOR, kVectorI64, int64_t) // Data::kVectorI64
    DEFINE_DATATYPE(TENSOR, kTensorFP32, float) // Data::kTensorFP32
    DEFINE_DATATYPE(TENSOR, kTensorI32, int32_t) // Data::kTensorI32
};

} // namespace core
} // namespace tcc

#undef DEFINE_DATATYPE
#undef SCALAR
#undef VECTOR
#undef TENSOR

#endif // TCC_DATA_H
