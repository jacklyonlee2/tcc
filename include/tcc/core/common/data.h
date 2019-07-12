#ifndef TCC_COMMON_DATA_H
#define TCC_COMMON_DATA_H

#include <vector>

#include "tcc/core/common/util.h"
#include "tcc/core/common/logging.h"

#define DECLARE_DATATYPE(type_enum, type) \
    public: \
        Data(type content_) : DataDesc(DataType::type_enum, {}) { \
            type##_content = { content_ }; \
        } \
        Data(std::vector<type> content_) : \
            DataDesc(DataType::type_enum, { static_cast<long>(content_.size()) }) { \
            CHECK(!content_.empty()) << \
                "Can not construct data with empty content."; \
            type##_content = content_; \
        } \
        Data(std::vector<type> content_, std::vector<long> data_shape_) : \
            DataDesc(DataType::type_enum, data_shape_) { \
            CHECK(!content_.empty()) << \
                "Can not construct data with empty content."; \
            CHECK(static_cast<long>(content_.size()) == \
                    accumulate_vector<long>(data_shape_)) << \
                "Content size must agree with shape size."; \
            type##_content = content_; \
        } \
        std::vector<type> get_##type() const { \
            CHECK(data_type == DataType::type_enum) << \
                "Requesting data with mismatching data type."; \
            return type##_content; \
        } \
    private: \
        std::vector<type> type##_content;

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
            data_type(data_type_) { set_shape(data_shape_); }

        DataType get_type() const { return data_type; }
        std::vector<long> get_shape() const { return data_shape; }
        unsigned int get_rank() const { return data_shape.size(); }

        bool defined() const { return data_type != DataType::UNINITIALIZED; }
        bool scalar() const { return data_shape.empty(); }

        void set_shape(std::vector<long> shape) {
            for (long dim : shape) {
                CHECK(dim > 0) <<
                    "Tensor shape can not contain negative/zero dimensions.";
            }
            data_shape = shape;
        }

    protected:
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

#undef DECLARE_DATATYPE

#endif // TCC_COMMON_DATA_H
