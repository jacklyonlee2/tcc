#ifndef TCC_DATA_H
#define TCC_DATA_H

#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>

#include "tcc/core/logging.h"

#define SHAPE_CHECK(shape) \
    CHECK(!shape.empty()) << "Tensor shape can not be empty."; \
    for (long dim : shape) { \
        CHECK(dim > 0) << \
            "Tensor shape can not contain negative/zero dimensions."; \
    }
#define SHAPE_ACCUM(shape) \
    std::accumulate( \
            shape.begin(), \
            shape.end(), \
            1, std::multiplies<long>());

#define DEFINE_DATATYPE(metatype, dt_enum, type) \
    metatype(dt_enum, type)
#define SCALAR(dt_enum, type) \
    public: \
        static Data dt_enum() { \
            return Data(Datatype::dt_enum, {}); \
        } \
        static Data dt_enum(const type data) { \
            return Data(data); \
        } \
        type Get##dt_enum() { \
            CHECK(!Empty()) << "Data is empty."; \
            return dt_enum##_; \
        } \
    private: \
        Data(const type dt_enum) : \
            datatype_(Datatype::dt_enum), shape_({}), \
            empty_(false), dt_enum##_(dt_enum) {} \
        type dt_enum##_;
#define TENSOR(dt_enum, type) \
    public: \
        static Data dt_enum(const long size) { \
            return Data::dt_enum(std::vector<long>({size})); \
        } \
        static Data dt_enum(const type* data, const long size) { \
            return Data::dt_enum(data, std::vector<long>({size})); \
        } \
        static Data dt_enum(const std::vector<long> shape) { \
            SHAPE_CHECK(shape); \
            return Data(Datatype::dt_enum, shape); \
        } \
        static Data dt_enum(const type* data, const std::vector<long> shape) { \
            SHAPE_CHECK(shape); \
            long size = SHAPE_ACCUM(shape); \
            std::vector<type> vec(data, data + static_cast<size_t>(size)); \
            return Data(shape, vec); \
        } \
        static Data dt_enum(const char* data, const std::vector<long> shape) { \
            return Data::dt_enum(reinterpret_cast<type*>(strdup(data)), shape); \
        } \
        std::vector<type> Get##dt_enum() { \
            CHECK(!Empty()) << "Data is empty."; \
            CHECK(datatype_ == Datatype::dt_enum) << "Datatype mismatch."; \
            return *dt_enum##_; \
        } \
    private: \
        Data(std::vector<long> shape, std::vector<type> dt_enum) : \
            datatype_(Datatype::dt_enum), shape_(shape), \
            empty_(false), dt_enum##_(std::make_shared<std::vector<type>>(dt_enum)) {} \
        std::shared_ptr<std::vector<type>> dt_enum##_;


namespace tcc {
namespace core {

enum class Datatype {
    kUninitialized,
    kScalarSTR,
    kScalarFP32,
    kTensorI32,
    kTensorI64,
    kTensorFP32
};

class Data {
    public:
        Data() : datatype_(Datatype::kUninitialized), empty_(true) {}
        Data(Datatype datatype, const std::vector<long> shape) :
            datatype_(datatype), shape_(shape), empty_(true) {}

        bool Initialized() const { return datatype_ != Datatype::kUninitialized; }
        bool Empty() const { return empty_; }

        Datatype GetType() const { return datatype_; }
        std::vector<long> GetShape() const { return shape_; }
        long GetSize() const { return SHAPE_ACCUM(shape_); }

    private:
        Datatype datatype_;
        const std::vector<long> shape_;
        const bool empty_;

    DEFINE_DATATYPE(SCALAR, kScalarSTR, std::string) // Data::kScalarSTR
    DEFINE_DATATYPE(SCALAR, kScalarFP32, float) // Data::kScalarFP32
    DEFINE_DATATYPE(TENSOR, kTensorI32, int32_t) // Data::kTensorI32
    DEFINE_DATATYPE(TENSOR, kTensorI64, int64_t) // Data::kTensorI64
    DEFINE_DATATYPE(TENSOR, kTensorFP32, float) // Data::kTensorFP32
};

} // namespace core
} // namespace tcc

#undef SHAPE_CHECK
#undef SHAPE_ACCUM
#undef DEFINE_DATATYPE
#undef SCALAR
#undef VECTOR
#undef TENSOR

#endif // TCC_DATA_H
