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
        static Data type_enum( \
                std::vector<data_type> content, \
                std::vector<long> data_shape) { \
            CHECK(!content.empty()) << \
                "Can not construct data with empty content."; \
            CHECK(content.size() == SHAPE_SIZE(data_shape)) << \
                "Content size must agree with shape size."; \
            Data data = Data(DataType::type_enum, data_shape); \
            data.type_enum##_content = content; \
            return data; \
        } \
        static Data type_enum( \
                data_type content) { \
            return Data::type_enum({content}, {}); \
        } \
        static Data type_enum( \
                const char *content_str, \
                std::vector<long> data_shape) { \
            const data_type *content_ptr = \
                reinterpret_cast<data_type*>( \
                        strdup(content_str)); \
            std::vector<data_type> content( \
                    content_ptr, \
                    content_ptr + static_cast<size_t>( \
                        SHAPE_SIZE(data_shape))); \
            return Data::type_enum(content, data_shape); \
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

        bool defined() const { return data_type != DataType::UNINITIALIZED; }
        bool scalar() const { return data_shape.empty(); }

    private:
        DataType data_type;
        std::vector<long> data_shape;
};


/* Data class stores constant scalar and tensor data and
 * related metadata. */
class Data : public DataDesc {
    public:
        Data() : DataDesc() {}

    private:
        Data(DataType data_type_, std::vector<long> data_shape_) :
            DataDesc(data_type_, data_shape_) {}

    DECLARE_DATATYPE(BOOL, bool)
    DECLARE_DATATYPE(INT, int)
    DECLARE_DATATYPE(LONG, long)
    DECLARE_DATATYPE(FLOAT, float)
};

} // namespace core
} // namespace tcc

#undef SHAPE_CHECK
#undef SHAPE_SIZE
#undef DECLARE_TYPE

#endif // TCC_COMMON_TENSOR_H
