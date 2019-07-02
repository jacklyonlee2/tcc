#ifndef TCC_COMMON_TENSOR_H
#define TCC_COMMON_TENSOR_H

#include <vector>
#include <numeric>

#include "tcc/core/common/logging.h"

namespace tcc {
namespace core {

enum class TensorType {
    UNINITIALIZED,
    INT,
    FLOAT
};

#define SHAPE_CHECK(shape) \
    for (long dim : shape) { \
        CHECK(dim > 0) << \
            "Tensor shape can not contain negative/zero dimensions."; \
    }

#define SHAPE_SIZE(shape) \
    std::accumulate( \
            shape.begin(), \
            shape.end(), \
            1l, std::multiplies<long>())

class TensorDesc {
    public:
        TensorDesc() : type(TensorType::UNINITIALIZED) {}
        TensorDesc(TensorType type_, std::vector<long> shape_) :
            type(type_), shape(shape_) { SHAPE_CHECK(shape_); }

        bool defined() { return type != TensorType::UNINITIALIZED; }
        TensorType get_type() { return type; }

    protected:
        TensorType type = TensorType::UNINITIALIZED;
        std::vector<long> shape;
};

#define DECLARE_TYPE(tensor_type, data_type) \
    public: \
        static Tensor tensor_type( \
                std::vector<data_type> content_, \
                std::vector<long> shape_) { \
            CHECK(!content_.empty()) << \
                "Can not construct Tensor with empty content."; \
            CHECK(static_cast<long>(content_.size()) == SHAPE_SIZE(shape_)) << \
                "Tensor shape must agree with tensor content size."; \
            Tensor t = Tensor(TensorType::tensor_type, shape_); \
            t.tensor_type##_content = content_; \
            return t; \
        } \
        static Tensor tensor_type( \
                const char* content_, \
                std::vector<long> shape_) { \
            long size = SHAPE_SIZE(shape_); \
            const data_type *raw = reinterpret_cast<data_type*>(strdup(content_)); \
            std::vector<data_type> vec(raw, raw + static_cast<size_t>(size)); \
            return Tensor::tensor_type(vec, shape_); \
        } \
    private: \
        std::vector<data_type> tensor_type##_content;

class Tensor : TensorDesc {
    public:
        Tensor() : TensorDesc() {}
        Tensor(TensorType type_, std::vector<long> shape_) :
            TensorDesc(type_, shape_) {}

    DECLARE_TYPE(INT, int)
    DECLARE_TYPE(FLOAT, float)
};

#undef SHAPE_CHECK
#undef SHAPE_SIZE
#undef DECLARE_TYPE

} // namespace core
} // namespace tcc

#endif // TCC_COMMON_TENSOR_H
