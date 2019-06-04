#ifndef TCC_DATATYPE_H
#define TCC_DATATYPE_H

namespace tcc {
namespace core {
namespace common {

enum class Datatype {
    kString,
    kScalarFloat32,
    kListInt64,
    kTensorFloat32
};

} // namespace common
} // namespace core
} // namespace tcc

#endif // TCC_DATATYPE_H
