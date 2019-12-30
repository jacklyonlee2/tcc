#ifndef TCC_FRONTEND_PARSE
#define TCC_FRONTEND_PARSE

#include "tcc/hlir/ir.h"
#include <unordered_map>
#include <vector>

namespace tcc {
namespace frontend {

/* deserializes the tensorflow frozen graph and parses it into hlir.
 * the parser uses provided input placeholder shapes to perform static
 * shape inference as hlir is constructed from the tensorflow graph. */
hlir::expr parse(std::string,
                 std::unordered_map<std::string, std::vector<long>>&);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_PARSE
