#ifndef TCC_FRONTEND_H
#define TCC_FRONTEND_H

#include "tcc/core/hlir.h"

namespace tcc {
namespace frontend {

core::HLIR FromTensorFlow(std::string input_path,
        std::unordered_map<std::string, std::vector<long>> input_shapes);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_H
