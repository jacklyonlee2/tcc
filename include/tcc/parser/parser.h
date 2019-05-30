#ifndef TCC_PARSER_H
#define TCC_PARSER_H

#include <string>

#include "tcc/core/hlir/hlir.h"

namespace tcc {
namespace parser {

void ParseFrozenGraph(core::hlir::HLIR& hlir, std::string file_path);

} // namespace parser
} // namespace tcc

#endif // TCC_PARSER_H
