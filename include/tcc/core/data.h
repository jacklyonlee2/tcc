#ifndef TCC_DATA_H
#define TCC_DATA_H

#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>

#include "tcc/core/logging.h"

#define DEFINE_DATATYPE(metatype, name, type) \
    metatype(name, type)
#define SCALAR(name, type) \
    private: \
        explicit Data(const type name) : \
            datatype_(Datatype::name), \
            name##_(name) {} \
        type name##_; \
    public: \
        static Data name(const type name) { return Data(name); }
#define LIST(name, type) \
    private: \
        explicit Data(const type* name, const int64_t length) : \
            datatype_(Datatype::name), \
            data_length_(length), \
            name##_(name) {} \
        const type* name##_; \
    public: \
        static Data name(const type* name, const int64_t length) { \
            CHECK(length != 0) << \
                "Data length can not be 0."; \
            return Data(name, length); \
        }
#define TENSOR(name, type) \
    private: \
        explicit Data( \
                const type* name, \
                const int64_t length, \
                const std::vector<int64_t> tensorshape) : \
            datatype_(Datatype::name), \
            data_length_(length), \
            tensorshape_(tensorshape), \
            name##_(name) {} \
        const type* name##_; \
    public: \
        static Data name(const type* name, const std::vector<int64_t> tensorshape) { \
            CHECK(!tensorshape.empty()) << \
                "Tensor shape can not be empty."; \
            CHECK_KEY_NOT_IN_VEC(0, tensorshape) << \
                "Tensor shape can not contain dimension of 0."; \
            int64_t length = std::accumulate( \
                    tensorshape.begin(), \
                    tensorshape.end(), \
                    1, std::multiplies<int64_t>()); \
            return Data(name, length, tensorshape); \
        } \
        static Data name(const char* name, const std::vector<int64_t> tensorshape) { \
            return Data::name(reinterpret_cast<type*>(strdup(name)), tensorshape); \
        }

namespace tcc {
namespace core {

enum class Datatype {
    kUninitialized,
    kString,
    kFloat32,
    kListInt64,
    kTensorFloat32,
    kTensorInt32
};

class Data {
    public:
        Data() {}
        bool IsEmpty() const { return datatype_ == Datatype::kUninitialized; }
        Datatype GetType() const { return datatype_; }

    private:
        Datatype datatype_ = Datatype::kUninitialized;
        int64_t data_length_;
        std::vector<int64_t> tensorshape_;

    DEFINE_DATATYPE(SCALAR, kString, std::string) // Data::kString
    DEFINE_DATATYPE(SCALAR, kFloat32, float) // Data::kScalarFloat
    DEFINE_DATATYPE(LIST, kListInt64, int64_t) // Data::kListInt64
    DEFINE_DATATYPE(TENSOR, kTensorFloat32, float) // Data::kTensorFloat32
    DEFINE_DATATYPE(TENSOR, kTensorInt32, int32_t) // Data::kTensorInt32
};

} // namespace core
} // namespace tcc

#undef DEFINE_DATATYPE
#undef SCALAR
#undef LIST
#undef TENSOR

#endif // TCC_DATA_H
