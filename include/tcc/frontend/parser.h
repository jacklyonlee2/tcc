#ifndef TCC_FRONTEND_PARSER_H
#define TCC_FRONTEND_PARSER_H

#include "tcc/core/ir.h"
#include <unordered_map>

namespace tcc {

/* parse deserializes the tensorflow frozen graph and parses it into core ir. */
expr parse(const std::string, std::unordered_map<std::string, dimensions>&);

} // namespace tcc

#endif // TCC_FRONTEND_PARSER_H
