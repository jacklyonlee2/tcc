#ifndef TCC_LLIR_H
#define TCC_LLIR_H

#include <memory>
#include <unordered_map>

namespace tcc {
namespace core {

class LLIR {
    public:
        class Primitive;
        typedef std::shared_ptr<Primitive> PrimitivePtr;

        class Fragment {
            public:
                const Data data_;

                const PrimitivePtr prev_pmt_;

            private:
                std::unordered_set<PrimitivePtr> next_pmts_;
        };

        typedef std::shared_ptr<Fragment> FragmentPtr;
        typedef std::weak_ptr<Fragment> WeakFragmentPtr;
        typedef std::vector<std::pair<long, long>> AccessPattern;

        class Primitive {
            private:
                std::vector<AccessPattern> input_access_patterns_;

                std::vector<WeakFragmentPtr> input_fragments_;
                WeakFragmentPtr output_fragment_;
        };

        LLIR(std::unordered_map<std::string, FragmentPtr> fragment_map,
                std::unordered_map<std::string, PrimitivePtr> primitive_map);

        void Print(std::ofstream& stream) const;

    private:
        std::unordered_map<std::string, FragmentPtr> fragment_map_;
        std::unordered_map<std::string, PrimitivePtr> primitive_map_;
};

} // namespace core
} // namespace tcc

#endif // TCC_LLIR_H
