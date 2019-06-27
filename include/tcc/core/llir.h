#ifndef TCC_LLIR_H
#define TCC_LLIR_H

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "tcc/core/data.h"

namespace tcc {
namespace core {

class LLIR {
    public:
        class Expression;
        class Primitive;
        class Fragment;
        typedef std::shared_ptr<Primitive> PrimitivePtr;
        typedef std::shared_ptr<Fragment> FragmentPtr;
        typedef std::weak_ptr<Fragment> WeakFragmentPtr;

        class Expression {
             public:
                 explicit Expression(int value);
                 explicit Expression(unsigned long range);
        };

        class Fragment {
            public:
                Fragment(const std::string instance_name, Data data);
                Fragment(const std::string instance_name, Data data,
                        PrimitivePtr prev_pmt);

                const std::string instance_name_;
                const Data data_;

                std::vector<long> GetShape() const;

            private:
                PrimitivePtr prev_pmt_;
                std::unordered_set<PrimitivePtr> next_pmts_;
        };

        class Primitive {
            private:
                std::vector<WeakFragmentPtr> input_fragments_;
                WeakFragmentPtr output_fragment_;
        };

        void Print(std::ofstream& stream) const;

    private:
        std::unordered_map<std::string, FragmentPtr> fragment_map_;
        std::unordered_map<std::string, PrimitivePtr> primitive_map_;
};

} // namespace core
} // namespace tcc

#endif // TCC_LLIR_H
