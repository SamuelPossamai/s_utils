
#ifndef S_UTILS_METALIST_H
#define S_UTILS_METALIST_H

#include <cstddef>

namespace sutils {

template<typename... Args>
class MetaList {

    template <std::size_t N, typename T, typename... MethodArgs> struct _get;
    template <std::size_t N, template <typename> typename T> struct _apply_aux;

public:

    static constexpr std::size_t size() { return sizeof...(Args); }

    template <std::size_t N>
    struct get {

        using Result = typename _get<N, Args...>::Result;
    };

    template <template <typename> typename T>
    static constexpr void applyToAll() {

        _apply_aux<0, T>::_apply_to_all();
    }

private:

    template <std::size_t N, typename T, typename... MethodArgs>
    struct _get {

        using Result = typename _get<N-1, MethodArgs...>::Result;
    };

    template <typename T, typename... MethodArgs>
    struct _get <0, T, MethodArgs...> {

        using Result = T;
    };

    template <std::size_t N, template <typename> typename T>
    struct _apply_aux {

        static constexpr void _apply_to_all() {

            T<typename get<N>::Result >();
            _apply_aux<N+1, T>::_apply_to_all();
        }
    };

    template <template <typename> typename T>
    struct _apply_aux <size(), T> {

        static constexpr void _apply_to_all() {}
    };
};

} /* namespace sutils */

#endif // S_UTILS_METALIST_H
