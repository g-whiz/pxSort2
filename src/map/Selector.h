#ifndef PXSORT2_SELECTOR_H
#define PXSORT2_SELECTOR_H

#include "common.h"
#include "typelist.h"

namespace pxsort::expr {

    /**
     * A function that takes D elements of type T, and returns a specific
     * element (e.g. the first one, or the 5th one) by value.
     * @tparam D the number of arguments taken by the selector
     * @tparam T the type of the arguments taken and returned by the selector
     */
    template<typename T, size_t D>
    class Selector {
    private:
        template<size_t N, typename = std::tuple<>>
        struct _arg_types_helper {};
        
        template<size_t N, typename... Args>
        struct _arg_types_helper<N, std::tuple<Args...>> {
            using Type = typename
                    _arg_types_helper<N - 1, std::tuple<T, Args...>>::Type;
        };

        template<typename... Args>
        struct _arg_types_helper<0, Args...> {
            using Type = std::tuple<Args...>;
        };

        using args_t = typename _arg_types_helper<D, std::tuple<>>::Type;

        template<size_t idx = D, typename TT = T, size_t DD = D>
        struct copy {
            static inline void args_to_array(args_t& args, std::array<TT, DD> a) {
                a[idx] = std::get<idx>(args);
                copy<idx - 1>::args_to_array(args, a);
            }
        };

        template<typename TT, size_t DD>
        struct copy<0, TT, DD> {
            static inline void args_to_array(args_t &args, std::array<TT, DD> a) {
                a[0] = std::get<0>(args);
            }
        };

    public:
        const uint8_t dimension = D;
        const uint8_t selection;

        Selector(uint8_t selection) : selection(selection % D) {}

        T operator()(args_t args) {
            std::array<T, D> array;
            copy<>::args_to_array(args, array);
            return array[selection];
        }
    };

    template <> class Selector<float, 3>;



//    template<typename T, size_t D, typename = T()>
//    struct _select_element_type_helper {};
//
//    template<typename T, size_t D, typename... Args>
//    struct _select_element_type_helper<T, D, T(Args...)> {
//        using Type = 
//            typename _select_element_type_helper<T, D - 1, T(T, Args...)>::Type;
//    };
//
//    template<typename T, typename... Args>
//    struct _select_element_type_helper<T, 0, T(Args...)> {
//        using Type = T(Args...);
//    };
//    
//    template<typename T, size_t D, typename F = T()>
//    using _select_element_t = 
//        typename _select_element_type_helper<T, D, F>::Type;
//
//    template<typename T, size_t D>
//    std::function<_select_element_t<T, D>> select_element(size_t element) {
//        return [=](T e, ...) {
//
//        }
//    }
}

#endif //PXSORT2_SELECTOR_H
