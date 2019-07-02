#ifndef TCC_FRONTEND
#define TCC_FRONTEND

#include <vector>
#include <unordered_map>

#include "tcc/core/hlir/hlir.h"

namespace tcc {
namespace frontend {

/* Function to parse TensorFlow frozen graph to HLIR */
core::HLIR parse_tensorflow(
        std::string input_path,
        std::unordered_map<std::string, std::vector<long>> input_shapes);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND
