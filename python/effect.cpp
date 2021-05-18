#include <boost/python/module.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/class.hpp>
#include <boost/python/pure_virtual.hpp>

#include "Effect.h"

using namespace pxsort;
using namespace boost::python;

class EffectWrap : public Effect, public wrapper<Effect> {
    void attach(Segment &tile) override {
        this->get_override("attach")(tile);
    }

    std::unique_ptr<Effect> clone() override {
        return this->get_override("clone")();
    }
};



BOOST_PYTHON_MODULE(effect) {

    class_<EffectWrap, std::unique_ptr<Effect>, boost::noncopyable>
            ("Effect", no_init);


}