#ifndef TCC_COMMON_TENSOR_H
#define TCC_COMMON_TENSOR_H

#include <vector>
#include <numeric>

#include "tcc/core/common/logging.h"

#define SHAPE_CHECK(shape) \
    for (long dim : shape) { \
        CHECK(dim > 0) << \
            "Tensor shape can not contain negative/zero dimensions."; \
    }

#define SHAPE_SIZE(shape) \
    static_cast<unsigned long>( \
            std::accumulate( \
                shape.begin(), \
                shape.end(), \
                1l, std::multiplies<long>()))

#define DECLARE_DATATYPE(type_enum, data_type) \
    public: \
        Data(data_type content_) : DataDesc(DataType::type_enum, {}) { \
            type_enum##_content = { content_ }; \
        } \
        Data(std::vector<data_type> content_, std::vector<long> data_shape_) : \
            DataDesc(DataType::type_enum, data_shape_) { \
            CHECK(!content_.empty()) << \
                "Can not construct data with empty content."; \
            CHECK(content_.size() == SHAPE_SIZE(data_shape_)) << \
                "Content size must agree with shape size."; \
            type_enum##_content = content_; \
        } \
    private: \
        std::vector<data_type> type_enum##_content;

namespace tcc {
namespace core {

enum class DataType {
    UNINITIALIZED,
    BOOL,
    INT,
    LONG,
    FLOAT
};

/* DataDesc class stores metadata related to scalar and
 * tensor data. */
class DataDesc {
    public:
        DataDesc() : data_type(DataType::UNINITIALIZED) {}
        DataDesc(DataType data_type_) : data_type(data_type_) {}
        DataDesc(DataType data_type_, std::vector<long> data_shape_) :
            data_type(data_type_), data_shape(data_shape_) { SHAPE_CHECK(data_shape_); }

        DataType get_type() const { return data_type; }
        std::vector<long> get_shape() const { return data_shape; }
        unsigned int get_rank() const { return data_shape.size(); }

        bool defined() const { return data_type != DataType::UNINITIALIZED; }

        void set_shape(std::vector<long> shape) {
            SHAPE_CHECK(shape);
            data_shape = shape;
        }

    private:
        DataType data_type;
        std::vector<long> data_shape;
};


/* Data class stores constant scalar and tensor data and
 * related metadata. */
class Data : public DataDesc {
    using DataDesc::DataDesc;

    DECLARE_DATATYPE(BOOL, bool)
    DECLARE_DATATYPE(INT, int)
    DECLARE_DATATYPE(LONG, long)
    DECLARE_DATATYPE(FLOAT, float)
};

} // namespace core
} // namespace tcc

#undef SHAPE_CHECK
#undef SHAPE_SIZE
#undef DECLARE_DATATYPE

#endif // TCC_COMMON_TENSOR_H
