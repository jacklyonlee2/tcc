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
        };

        typedef std::shared_ptr<Fragment> FragmentPtr;

        class Primitive {
        };

    private:
        std::unordered_map<std::string, FragmentPtr> fragment_map_;
        std::unordered_map<std::string, PrimitivePtr> primitive_map_;
};

} // namespace core
} // namespace tcc

#endif // TCC_LLIR_H
