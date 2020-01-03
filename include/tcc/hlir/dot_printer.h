#ifndef TCC_HLIR_DOT_PRINTER_H
#define TCC_HLIR_DOT_PRINTER_H

#include "tcc/hlir/visitor.h"
#include <unordered_map>

namespace tcc {
namespace hlir {

/* generate a graphviz DOT file
 * corresponding to the given hlir. */
struct dot_printer : visitor_base
{
  public:
    static void apply(std::string, expr);

  protected:
    void visit(var_expr) override;
    void visit(cnst_expr) override;
    void visit(range_expr) override;
    void visit(index_expr) override;
    void visit(select_expr) override;
    void visit(reshape_expr) override;
    void visit(exp_expr) override;
    void visit(sqrt_expr) override;
    void visit(add_expr) override;
    void visit(sub_expr) override;
    void visit(mul_expr) override;
    void visit(div_expr) override;
    void visit(mod_expr) override;
    void visit(greater_expr) override;
    void visit(greater_equal_expr) override;
    void visit(less_expr) override;
    void visit(logical_and_expr) override;
    void visit(reduce_expr) override;

    struct graph
    {
      public:
        void add_node(expr, std::string, std::vector<std::string> = {});
        void add_edge(expr, expr, std::string);
        std::string str() const;

      private:
        struct node
        {
          public:
            node(expr,
                 std::string,
                 data_type,
                 dimensions,
                 std::vector<std::string>);
            std::string get_input_label(std::string) const;
            std::string str() const;

          private:
            std::string e;
            std::string etype;
            std::string dtype;
            std::string shape;
            std::vector<std::string> inputs;
            std::unordered_map<std::string, std::string> input_labels;
        };

        struct edge
        {
          public:
            edge(expr, expr, std::string);
            std::string str() const;

          private:
            std::string src;
            std::string dst;
            std::string dst_input_label;
        };

        std::unordered_map<expr, node> nodes;
        std::vector<edge> edges;
    };

    graph ir;
};

} // namespace hlir
} // namespace tcc

#endif // TCC_HLIR_DOT_PRINTER_H
