#ifndef TCC_FRONTEND_PARSER
#define TCC_FRONTEND_PARSER

#include "tcc/affn/ir.h"
#include <unordered_map>

namespace tcc {
namespace frontend {

/* parse deserializes the tensorflow frozen graph and parses it into affn ir. */
affn::expr parse(std::string, std::unordered_map<std::string, dimensions>&);

} // namespace frontend
} // namespace tcc

#endif // TCC_FRONTEND_PARSER
