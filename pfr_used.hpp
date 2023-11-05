// Copyright (c) 2016-2023 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PFR_HPP
#define BOOST_PFR_HPP

/// \file boost/pfr.hpp
/// Includes all the Boost.PFR headers

// Copyright (c) 2016-2023 Antony Polukhin
// Copyright (c) 2022 Denis Mikhailov
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PFR_CONFIG_HPP
    #define BOOST_PFR_CONFIG_HPP
    #pragma once

    #if __cplusplus >= 201402L || (defined(_MSC_VER) && defined(_MSVC_LANG) && _MSC_VER > 1900)
        #include <type_traits>  // to get non standard platform macro definitions (__GLIBCXX__ for example)
    #endif

    /// \file boost/pfr/config.hpp
    /// Contains all the macros that describe Boost.PFR configuration, like BOOST_PFR_ENABLED
    ///
    /// \note This header file doesn't require C++14 Standard and supports all C++ compilers, even pre C++14 compilers (C++11, C++03...).

    // Reminder:
    //  * MSVC++ 14.2 _MSC_VER == 1927 <- Loophole is known to work (Visual Studio ????)
    //  * MSVC++ 14.1 _MSC_VER == 1916 <- Loophole is known to NOT work (Visual Studio 2017)
    //  * MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
    //  * MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)

    #ifdef BOOST_PFR_NOT_SUPPORTED
        #error Please, do not set BOOST_PFR_NOT_SUPPORTED value manually, use '-DBOOST_PFR_ENABLED=0' instead of it
    #endif

    #if defined(_MSC_VER)
        #if !defined(_MSVC_LANG) || _MSC_VER <= 1900
            #define BOOST_PFR_NOT_SUPPORTED 1
        #endif
    #elif __cplusplus < 201402L
        #define BOOST_PFR_NOT_SUPPORTED 1
    #endif

    #ifndef BOOST_PFR_USE_LOOPHOLE
        #if defined(_MSC_VER)
            #if _MSC_VER >= 1927
                #define BOOST_PFR_USE_LOOPHOLE 1
            #else
                #define BOOST_PFR_USE_LOOPHOLE 0
            #endif
        #elif defined(__clang_major__) && __clang_major__ >= 8
            #define BOOST_PFR_USE_LOOPHOLE 0
        #else
            #define BOOST_PFR_USE_LOOPHOLE 1
        #endif
    #endif

    #ifndef BOOST_PFR_USE_CPP17
        #ifdef __cpp_structured_bindings
            #define BOOST_PFR_USE_CPP17 1
        #elif defined(_MSVC_LANG)
            #if _MSVC_LANG >= 201703L
                #define BOOST_PFR_USE_CPP17 1
            #else
                #define BOOST_PFR_USE_CPP17 0
            #endif
        #else
            #define BOOST_PFR_USE_CPP17 0
        #endif
    #endif

    #if (!BOOST_PFR_USE_CPP17 && !BOOST_PFR_USE_LOOPHOLE)
        #if (defined(_MSC_VER) && _MSC_VER < 1916)  ///< in Visual Studio 2017 v15.9 PFR library with classic engine normally works
            #define BOOST_PFR_NOT_SUPPORTED 1
        #endif
    #endif

    #ifndef BOOST_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE
        // Assume that libstdc++ since GCC-7.3 does not have linear instantiation depth in std::make_integral_sequence
        #if defined(__GLIBCXX__) && __GLIBCXX__ >= 20180101
            #define BOOST_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE 1
        #elif defined(_MSC_VER)
            #define BOOST_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE 1
        //# elif other known working lib
        #else
            #define BOOST_PFR_USE_STD_MAKE_INTEGRAL_SEQUENCE 0
        #endif
    #endif

    #ifndef BOOST_PFR_HAS_GUARANTEED_COPY_ELISION
        #if defined(__cpp_guaranteed_copy_elision) && (!defined(_MSC_VER) || _MSC_VER > 1928)
            #define BOOST_PFR_HAS_GUARANTEED_COPY_ELISION 1
        #else
            #define BOOST_PFR_HAS_GUARANTEED_COPY_ELISION 0
        #endif
    #endif

    #ifndef BOOST_PFR_ENABLE_IMPLICIT_REFLECTION
        #if defined(__cpp_lib_is_aggregate)
            #define BOOST_PFR_ENABLE_IMPLICIT_REFLECTION 1
        #else
            // There is no way to detect potential ability to be reflectable without std::is_aggregare
            #define BOOST_PFR_ENABLE_IMPLICIT_REFLECTION 0
        #endif
    #endif

    #ifndef BOOST_PFR_CORE_NAME_ENABLED
        #if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L))
            #if (defined(__cpp_nontype_template_args) && __cpp_nontype_template_args >= 201911) || (defined(__clang_major__) && __clang_major__ >= 12)
                #define BOOST_PFR_CORE_NAME_ENABLED 1
            #else
                #define BOOST_PFR_CORE_NAME_ENABLED 0
            #endif
        #else
            #define BOOST_PFR_CORE_NAME_ENABLED 0
        #endif
    #endif

    #ifndef BOOST_PFR_CORE_NAME_PARSING
        #if defined(_MSC_VER)
            #define BOOST_PFR_CORE_NAME_PARSING (sizeof("auto __cdecl boost::pfr::detail::name_of_field_impl<") - 1, sizeof(">(void) noexcept") - 1, backward("->"))
        #elif defined(__clang__)
            #define BOOST_PFR_CORE_NAME_PARSING (sizeof("auto boost::pfr::detail::name_of_field_impl() [MsvcWorkaround = ") - 1, sizeof("}]") - 1, backward("."))
        #elif defined(__GNUC__)
            #define BOOST_PFR_CORE_NAME_PARSING (sizeof("consteval auto boost::pfr::detail::name_of_field_impl() [with MsvcWorkaround = ") - 1, sizeof(")]") - 1, backward("::"))
        #else
            // Default parser for other platforms... Just skip nothing!
            #define BOOST_PFR_CORE_NAME_PARSING (0, 0, "")
        #endif
    #endif

    #if defined(__has_cpp_attribute)
        #if __has_cpp_attribute(maybe_unused)
            #define BOOST_PFR_MAYBE_UNUSED [[maybe_unused]]
        #endif
    #endif

    #ifndef BOOST_PFR_MAYBE_UNUSED
        #define BOOST_PFR_MAYBE_UNUSED
    #endif

    #ifndef BOOST_PFR_ENABLED
        #ifdef BOOST_PFR_NOT_SUPPORTED
            #define BOOST_PFR_ENABLED 0
        #else
            #define BOOST_PFR_ENABLED 1
        #endif
    #endif

    #undef BOOST_PFR_NOT_SUPPORTED

#endif  // BOOST_PFR_CONFIG_HPP

namespace boost {
namespace pfr {
template<class T, class WhatFor>
struct is_reflectable;
}
}  // namespace boost
   //
   // Distributed under the Boost Software License, Version 1.0. (See accompanying
   // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PFR_CORE_HPP
    #define BOOST_PFR_CORE_HPP
    #pragma once

    // Copyright (c) 2016-2023 Antony Polukhin
    //
    // Distributed under the Boost Software License, Version 1.0. (See accompanying
    // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PFR_DETAIL_CORE17_HPP
        #define BOOST_PFR_DETAIL_CORE17_HPP
        #pragma once

        // Copyright (c) 2016-2023 Antony Polukhin
        // Copyright (c) 2023 Denis Mikhailov
        //
        // Distributed under the Boost Software License, Version 1.0. (See accompanying
        // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////// THIS HEADER IS AUTO GENERATED BY misc/generate_cpp17.py                                    ////////////////
        //////////////// MODIFY AND RUN THE misc/generate_cpp17.py INSTEAD OF DIRECTLY MODIFYING THE GENERATED FILE ////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        #ifndef BOOST_PFR_DETAIL_CORE17_GENERATED_HPP
            #define BOOST_PFR_DETAIL_CORE17_GENERATED_HPP
            #pragma once

            #if !BOOST_PFR_USE_CPP17
                #error C++17 is required for this header.
            #endif

            // Copyright (c) 2016-2023 Antony Polukhin
            //
            // Distributed under the Boost Software License, Version 1.0. (See accompanying
            // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

            #ifndef BOOST_PFR_DETAIL_SEQUENCE_TUPLE_HPP
                #define BOOST_PFR_DETAIL_SEQUENCE_TUPLE_HPP
                #pragma once

                #include <cstddef>  // std::size_t
                #include <utility>  // metaprogramming stuff

///////////////////// Tuple that holds its values in the supplied order
namespace boost {
namespace pfr {
namespace detail {
namespace sequence_tuple {

template<std::size_t N, class T>
struct base_from_member
{
    T value;
};

template<class I, class... Tail>
struct tuple_base;

template<std::size_t... I, class... Tail>
struct tuple_base<std::index_sequence<I...>, Tail...>
  : base_from_member<I, Tail>...
{
    static constexpr std::size_t size_v = sizeof...(I);

    // We do not use `noexcept` in the following functions, because if user forget to put one then clang will issue an error:
    // "error: exception specification of explicitly defaulted default constructor does not match the calculated one".
    constexpr tuple_base() = default;
    constexpr tuple_base(tuple_base&&) = default;
    constexpr tuple_base(const tuple_base&) = default;

    constexpr tuple_base(Tail... v) noexcept
      : base_from_member<I, Tail>{v}...
    {}
};

template<>
struct tuple_base<std::index_sequence<>>
{
    static constexpr std::size_t size_v = 0;
};

template<std::size_t N, class T>
constexpr T& get_impl(base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<std::size_t N, class T>
constexpr const T& get_impl(const base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<std::size_t N, class T>
constexpr volatile T& get_impl(volatile base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<std::size_t N, class T>
constexpr const volatile T& get_impl(const volatile base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<std::size_t N, class T>
constexpr T&& get_impl(base_from_member<N, T>&& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return std::forward<T>(t.value);
}

template<class T, std::size_t N>
constexpr T& get_by_type_impl(base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<class T, std::size_t N>
constexpr const T& get_by_type_impl(const base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<class T, std::size_t N>
constexpr volatile T& get_by_type_impl(volatile base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<class T, std::size_t N>
constexpr const volatile T& get_by_type_impl(const volatile base_from_member<N, T>& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return t.value;
}

template<class T, std::size_t N>
constexpr T&& get_by_type_impl(base_from_member<N, T>&& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return std::forward<T>(t.value);
}

template<class T, std::size_t N>
constexpr const T&& get_by_type_impl(const base_from_member<N, T>&& t) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
    return std::forward<T>(t.value);
}

template<class... Values>
struct tuple : tuple_base<
                   std::index_sequence_for<Values...>,
                   Values...>
{
    using tuple_base<
        std::index_sequence_for<Values...>,
        Values...>::tuple_base;

    constexpr static std::size_t size() noexcept
    {
        return sizeof...(Values);
    }
    constexpr static bool empty() noexcept
    {
        return size() == 0;
    }
};

template<std::size_t N, class... T>
constexpr decltype(auto) get(tuple<T...>& t) noexcept
{
    static_assert(N < tuple<T...>::size_v, "====================> Boost.PFR: Tuple index out of bounds");
    return sequence_tuple::get_impl<N>(t);
}

template<std::size_t N, class... T>
constexpr decltype(auto) get(const tuple<T...>& t) noexcept
{
    static_assert(N < tuple<T...>::size_v, "====================> Boost.PFR: Tuple index out of bounds");
    return sequence_tuple::get_impl<N>(t);
}

template<std::size_t N, class... T>
constexpr decltype(auto) get(const volatile tuple<T...>& t) noexcept
{
    static_assert(N < tuple<T...>::size_v, "====================> Boost.PFR: Tuple index out of bounds");
    return sequence_tuple::get_impl<N>(t);
}

template<std::size_t N, class... T>
constexpr decltype(auto) get(volatile tuple<T...>& t) noexcept
{
    static_assert(N < tuple<T...>::size_v, "====================> Boost.PFR: Tuple index out of bounds");
    return sequence_tuple::get_impl<N>(t);
}

template<std::size_t N, class... T>
constexpr decltype(auto) get(tuple<T...>&& t) noexcept
{
    static_assert(N < tuple<T...>::size_v, "====================> Boost.PFR: Tuple index out of bounds");
    return sequence_tuple::get_impl<N>(std::move(t));
}

template<std::size_t I, class T>
using tuple_element = std::remove_reference<decltype(::boost::pfr::detail::sequence_tuple::get<I>(std::declval<T>()))>;

template<class... Args>
constexpr auto make_sequence_tuple(Args... args) noexcept
{
    return ::boost::pfr::detail::sequence_tuple::tuple<Args...>{args...};
}

}  // namespace sequence_tuple
}  // namespace detail
}  // namespace pfr
}  // namespace boost

            #endif  // BOOST_PFR_CORE_HPP

            // Copyright (c) 2016-2023 Antony Polukhin
            //
            // Distributed under the Boost Software License, Version 1.0. (See accompanying
            // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

            #ifndef BOOST_PFR_DETAIL_SIZE_T_HPP
                #define BOOST_PFR_DETAIL_SIZE_T_HPP
                #pragma once

namespace boost {
namespace pfr {
namespace detail {

///////////////////// General utility stuff
template<std::size_t Index>
using size_t_ = std::integral_constant<std::size_t, Index>;

}  // namespace detail
}  // namespace pfr
}  // namespace boost

            #endif  // BOOST_PFR_DETAIL_SIZE_T_HPP

            #include <type_traits>  // for std::conditional_t, std::is_reference

namespace boost {
namespace pfr {
namespace detail {

template<class... Args>
constexpr auto make_tuple_of_references(Args&&... args) noexcept
{
    return sequence_tuple::tuple<Args&...>{args...};
}

template<typename T, typename Arg>
constexpr decltype(auto) add_cv_like(Arg& arg) noexcept
{
    if constexpr (std::is_const<T>::value && std::is_volatile<T>::value)
    {
        return const_cast<const volatile Arg&>(arg);
    }
    else if constexpr (std::is_const<T>::value)
    {
        return const_cast<const Arg&>(arg);
    }
    else if constexpr (std::is_volatile<T>::value)
    {
        return const_cast<volatile Arg&>(arg);
    }
    else
    {
        return const_cast<Arg&>(arg);
    }
}

// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78939
template<typename T, typename Sb, typename Arg>
constexpr decltype(auto) workaround_cast(Arg& arg) noexcept
{
    using output_arg_t = std::conditional_t<!std::is_reference<Sb>(), decltype(detail::add_cv_like<T>(arg)), Sb>;
    return const_cast<output_arg_t>(arg);
}

template<class T>
constexpr auto tie_as_tuple(T& /*val*/, size_t_<0>) noexcept
{
    return sequence_tuple::tuple<>{};
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<1>, std::enable_if_t<std::is_class<std::remove_cv_t<T>>::value>* = nullptr) noexcept
{
    auto& [a] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.
    return ::boost::pfr::detail::make_tuple_of_references(detail::workaround_cast<T, decltype(a)>(a));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<1>, std::enable_if_t<!std::is_class<std::remove_cv_t<T>>::value>* = nullptr) noexcept
{
    return ::boost::pfr::detail::make_tuple_of_references(val);
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<2>) noexcept
{
    auto& [a, b] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.
    return ::boost::pfr::detail::make_tuple_of_references(detail::workaround_cast<T, decltype(a)>(a), detail::workaround_cast<T, decltype(b)>(b));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<3>) noexcept
{
    auto& [a, b, c] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<4>) noexcept
{
    auto& [a, b, c, d] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<5>) noexcept
{
    auto& [a, b, c, d, e] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<6>) noexcept
{
    auto& [a, b, c, d, e, f] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<7>) noexcept
{
    auto& [a, b, c, d, e, f, g] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<8>) noexcept
{
    auto& [a, b, c, d, e, f, g, h] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<9>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<10>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<11>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<12>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<13>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<14>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<15>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<16>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<17>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<18>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<19>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<20>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<21>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<22>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<23>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<24>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<25>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<26>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<27>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<28>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<29>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<30>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<31>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<32>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<33>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<34>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<35>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<36>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<37>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<38>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<39>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<40>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<41>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<42>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<43>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<44>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<45>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<46>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<47>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<48>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z,
           aa] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<49>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<50>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<51>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<52>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<53>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<54>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<55>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<56>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<57>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<58>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<59>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<60>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<61>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<62>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<63>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<64>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<65>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<66>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<67>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<68>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<69>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<70>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<71>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<72>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<73>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<74>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<75>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<76>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<77>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<78>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<79>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<80>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<81>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<82>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<83>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<84>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<85>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<86>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<87>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<88>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<89>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<90>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<91>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<92>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<93>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<94>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY, aZ] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY),
        detail::workaround_cast<T, decltype(aZ)>(aZ));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<95>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY, aZ,
           ba] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY),
        detail::workaround_cast<T, decltype(aZ)>(aZ),
        detail::workaround_cast<T, decltype(ba)>(ba));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<96>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY, aZ, ba, bb] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY),
        detail::workaround_cast<T, decltype(aZ)>(aZ),
        detail::workaround_cast<T, decltype(ba)>(ba),
        detail::workaround_cast<T, decltype(bb)>(bb));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<97>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY, aZ, ba, bb, bc] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY),
        detail::workaround_cast<T, decltype(aZ)>(aZ),
        detail::workaround_cast<T, decltype(ba)>(ba),
        detail::workaround_cast<T, decltype(bb)>(bb),
        detail::workaround_cast<T, decltype(bc)>(bc));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<98>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY, aZ, ba, bb, bc, bd] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY),
        detail::workaround_cast<T, decltype(aZ)>(aZ),
        detail::workaround_cast<T, decltype(ba)>(ba),
        detail::workaround_cast<T, decltype(bb)>(bb),
        detail::workaround_cast<T, decltype(bc)>(bc),
        detail::workaround_cast<T, decltype(bd)>(bd));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<99>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY, aZ, ba, bb, bc, bd, be] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY),
        detail::workaround_cast<T, decltype(aZ)>(aZ),
        detail::workaround_cast<T, decltype(ba)>(ba),
        detail::workaround_cast<T, decltype(bb)>(bb),
        detail::workaround_cast<T, decltype(bc)>(bc),
        detail::workaround_cast<T, decltype(bd)>(bd),
        detail::workaround_cast<T, decltype(be)>(be));
}

template<class T>
constexpr auto tie_as_tuple(T& val, size_t_<100>) noexcept
{
    auto& [a, b, c, d, e, f, g, h, j, k, l, m, n, p, q, r, s, t, u, v, w, x, y, z, A, B, C, D, E, F, G, H, J, K, L, M, N, P, Q, R, S, U, V, W, X, Y, Z, aa, ab, ac, ad, ae, af, ag, ah, aj, ak, al, am, an, ap, aq, ar, as, at, au, av, aw, ax, ay, az, aA, aB, aC, aD, aE, aF, aG, aH, aJ, aK, aL, aM, aN, aP, aQ, aR, aS, aU, aV, aW, aX, aY, aZ, ba, bb, bc, bd, be, bf] = const_cast<std::remove_cv_t<T>&>(val);  // ====================> Boost.PFR: User-provided type is not a SimpleAggregate.

    return ::boost::pfr::detail::make_tuple_of_references(
        detail::workaround_cast<T, decltype(a)>(a),
        detail::workaround_cast<T, decltype(b)>(b),
        detail::workaround_cast<T, decltype(c)>(c),
        detail::workaround_cast<T, decltype(d)>(d),
        detail::workaround_cast<T, decltype(e)>(e),
        detail::workaround_cast<T, decltype(f)>(f),
        detail::workaround_cast<T, decltype(g)>(g),
        detail::workaround_cast<T, decltype(h)>(h),
        detail::workaround_cast<T, decltype(j)>(j),
        detail::workaround_cast<T, decltype(k)>(k),
        detail::workaround_cast<T, decltype(l)>(l),
        detail::workaround_cast<T, decltype(m)>(m),
        detail::workaround_cast<T, decltype(n)>(n),
        detail::workaround_cast<T, decltype(p)>(p),
        detail::workaround_cast<T, decltype(q)>(q),
        detail::workaround_cast<T, decltype(r)>(r),
        detail::workaround_cast<T, decltype(s)>(s),
        detail::workaround_cast<T, decltype(t)>(t),
        detail::workaround_cast<T, decltype(u)>(u),
        detail::workaround_cast<T, decltype(v)>(v),
        detail::workaround_cast<T, decltype(w)>(w),
        detail::workaround_cast<T, decltype(x)>(x),
        detail::workaround_cast<T, decltype(y)>(y),
        detail::workaround_cast<T, decltype(z)>(z),
        detail::workaround_cast<T, decltype(A)>(A),
        detail::workaround_cast<T, decltype(B)>(B),
        detail::workaround_cast<T, decltype(C)>(C),
        detail::workaround_cast<T, decltype(D)>(D),
        detail::workaround_cast<T, decltype(E)>(E),
        detail::workaround_cast<T, decltype(F)>(F),
        detail::workaround_cast<T, decltype(G)>(G),
        detail::workaround_cast<T, decltype(H)>(H),
        detail::workaround_cast<T, decltype(J)>(J),
        detail::workaround_cast<T, decltype(K)>(K),
        detail::workaround_cast<T, decltype(L)>(L),
        detail::workaround_cast<T, decltype(M)>(M),
        detail::workaround_cast<T, decltype(N)>(N),
        detail::workaround_cast<T, decltype(P)>(P),
        detail::workaround_cast<T, decltype(Q)>(Q),
        detail::workaround_cast<T, decltype(R)>(R),
        detail::workaround_cast<T, decltype(S)>(S),
        detail::workaround_cast<T, decltype(U)>(U),
        detail::workaround_cast<T, decltype(V)>(V),
        detail::workaround_cast<T, decltype(W)>(W),
        detail::workaround_cast<T, decltype(X)>(X),
        detail::workaround_cast<T, decltype(Y)>(Y),
        detail::workaround_cast<T, decltype(Z)>(Z),
        detail::workaround_cast<T, decltype(aa)>(aa),
        detail::workaround_cast<T, decltype(ab)>(ab),
        detail::workaround_cast<T, decltype(ac)>(ac),
        detail::workaround_cast<T, decltype(ad)>(ad),
        detail::workaround_cast<T, decltype(ae)>(ae),
        detail::workaround_cast<T, decltype(af)>(af),
        detail::workaround_cast<T, decltype(ag)>(ag),
        detail::workaround_cast<T, decltype(ah)>(ah),
        detail::workaround_cast<T, decltype(aj)>(aj),
        detail::workaround_cast<T, decltype(ak)>(ak),
        detail::workaround_cast<T, decltype(al)>(al),
        detail::workaround_cast<T, decltype(am)>(am),
        detail::workaround_cast<T, decltype(an)>(an),
        detail::workaround_cast<T, decltype(ap)>(ap),
        detail::workaround_cast<T, decltype(aq)>(aq),
        detail::workaround_cast<T, decltype(ar)>(ar),
        detail::workaround_cast<T, decltype(as)>(as),
        detail::workaround_cast<T, decltype(at)>(at),
        detail::workaround_cast<T, decltype(au)>(au),
        detail::workaround_cast<T, decltype(av)>(av),
        detail::workaround_cast<T, decltype(aw)>(aw),
        detail::workaround_cast<T, decltype(ax)>(ax),
        detail::workaround_cast<T, decltype(ay)>(ay),
        detail::workaround_cast<T, decltype(az)>(az),
        detail::workaround_cast<T, decltype(aA)>(aA),
        detail::workaround_cast<T, decltype(aB)>(aB),
        detail::workaround_cast<T, decltype(aC)>(aC),
        detail::workaround_cast<T, decltype(aD)>(aD),
        detail::workaround_cast<T, decltype(aE)>(aE),
        detail::workaround_cast<T, decltype(aF)>(aF),
        detail::workaround_cast<T, decltype(aG)>(aG),
        detail::workaround_cast<T, decltype(aH)>(aH),
        detail::workaround_cast<T, decltype(aJ)>(aJ),
        detail::workaround_cast<T, decltype(aK)>(aK),
        detail::workaround_cast<T, decltype(aL)>(aL),
        detail::workaround_cast<T, decltype(aM)>(aM),
        detail::workaround_cast<T, decltype(aN)>(aN),
        detail::workaround_cast<T, decltype(aP)>(aP),
        detail::workaround_cast<T, decltype(aQ)>(aQ),
        detail::workaround_cast<T, decltype(aR)>(aR),
        detail::workaround_cast<T, decltype(aS)>(aS),
        detail::workaround_cast<T, decltype(aU)>(aU),
        detail::workaround_cast<T, decltype(aV)>(aV),
        detail::workaround_cast<T, decltype(aW)>(aW),
        detail::workaround_cast<T, decltype(aX)>(aX),
        detail::workaround_cast<T, decltype(aY)>(aY),
        detail::workaround_cast<T, decltype(aZ)>(aZ),
        detail::workaround_cast<T, decltype(ba)>(ba),
        detail::workaround_cast<T, decltype(bb)>(bb),
        detail::workaround_cast<T, decltype(bc)>(bc),
        detail::workaround_cast<T, decltype(bd)>(bd),
        detail::workaround_cast<T, decltype(be)>(be),
        detail::workaround_cast<T, decltype(bf)>(bf));
}

template<class T, std::size_t I>
constexpr void tie_as_tuple(T& /*val*/, size_t_<I>) noexcept
{
    static_assert(sizeof(T) && false,
                  "====================> Boost.PFR: Too many fields in a structure T. Regenerate include/boost/pfr/detail/core17_generated.hpp file for appropriate count of fields. For example: `python ./misc/generate_cpp17.py 300 > include/boost/pfr/detail/core17_generated.hpp`");
}

}  // namespace detail
}  // namespace pfr
}  // namespace boost

        #endif  // BOOST_PFR_DETAIL_CORE17_GENERATED_HPP

        // Copyright (c) 2016-2023 Antony Polukhin
        //
        // Distributed under the Boost Software License, Version 1.0. (See accompanying
        // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

        #ifndef BOOST_PFR_DETAIL_FIELDS_COUNT_HPP
            #define BOOST_PFR_DETAIL_FIELDS_COUNT_HPP
            #pragma once

            // Copyright (c) 2019-2023 Antony Polukhin.
            //
            // Distributed under the Boost Software License, Version 1.0. (See accompanying
            // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

            #ifndef BOOST_PFR_DETAIL_UNSAFE_DECLVAL_HPP
                #define BOOST_PFR_DETAIL_UNSAFE_DECLVAL_HPP
                #pragma once

                #include <type_traits>

namespace boost {
namespace pfr {
namespace detail {

// This function serves as a link-time assert. If linker requires it, then
// `unsafe_declval()` is used at runtime.
void report_if_you_see_link_error_with_this_function() noexcept;

// For returning non default constructible types. Do NOT use at runtime!
//
// GCCs std::declval may not be used in potentionally evaluated contexts,
// so we reinvent it.
template<class T>
constexpr T unsafe_declval() noexcept
{
    report_if_you_see_link_error_with_this_function();

    typename std::remove_reference<T>::type* ptr = nullptr;
    ptr += 42;  // suppresses 'null pointer dereference' warnings
    return static_cast<T>(*ptr);
}

}  // namespace detail
}  // namespace pfr
}  // namespace boost

            #endif  // BOOST_PFR_DETAIL_UNSAFE_DECLVAL_HPP

            #include <climits>  // CHAR_BIT
            #include <type_traits>
            #include <utility>  // metaprogramming stuff

            #ifdef __clang__
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wmissing-braces"
                #pragma clang diagnostic ignored "-Wundefined-inline"
                #pragma clang diagnostic ignored "-Wundefined-internal"
                #pragma clang diagnostic ignored "-Wmissing-field-initializers"
            #endif

namespace boost {
namespace pfr {
namespace detail {

///////////////////// Structure that can be converted to reference to anything
struct ubiq_lref_constructor
{
    std::size_t ignore;
    template<class Type>
    constexpr operator Type&() const&& noexcept
    {  // tweak for template_unconstrained.cpp like cases
        return detail::unsafe_declval<Type&>();
    }

    template<class Type>
    constexpr operator Type&() const& noexcept
    {  // tweak for optional_chrono.cpp like cases
        return detail::unsafe_declval<Type&>();
    }
};

///////////////////// Structure that can be converted to rvalue reference to anything
struct ubiq_rref_constructor
{
    std::size_t ignore;
    template<class Type>
    /*constexpr*/ operator Type() const&& noexcept
    {  // Allows initialization of rvalue reference fields and move-only types
        return detail::unsafe_declval<Type>();
    }
};

            #ifndef __cpp_lib_is_aggregate
///////////////////// Hand-made is_aggregate_initializable_n<T> trait

// Structure that can be converted to reference to anything except reference to T
template<class T, bool IsCopyConstructible>
struct ubiq_constructor_except
{
    std::size_t ignore;
    template<class Type>
    constexpr operator std::enable_if_t<!std::is_same<T, Type>::value, Type&>() const noexcept;  // Undefined
};

template<class T>
struct ubiq_constructor_except<T, false>
{
    std::size_t ignore;
    template<class Type>
    constexpr operator std::enable_if_t<!std::is_same<T, Type>::value, Type&&>() const noexcept;  // Undefined
};

// `std::is_constructible<T, ubiq_constructor_except<T>>` consumes a lot of time, so we made a separate lazy trait for it.
template<std::size_t N, class T>
struct is_single_field_and_aggregate_initializable : std::false_type
{};
template<class T>
struct is_single_field_and_aggregate_initializable<1, T> : std::integral_constant<
                                                               bool,
                                                               !std::is_constructible<T, ubiq_constructor_except<T, std::is_copy_constructible<T>::value>>::value>
{};

// Hand-made is_aggregate<T> trait:
// Before C++20 aggregates could be constructed from `decltype(ubiq_?ref_constructor{I})...` but type traits report that
// there's no constructor from `decltype(ubiq_?ref_constructor{I})...`
// Special case for N == 1: `std::is_constructible<T, ubiq_?ref_constructor>` returns true if N == 1 and T is copy/move constructible.
template<class T, std::size_t N>
struct is_aggregate_initializable_n
{
    template<std::size_t... I>
    static constexpr bool is_not_constructible_n(std::index_sequence<I...>) noexcept
    {
        return (!std::is_constructible<T, decltype(ubiq_lref_constructor{I})...>::value && !std::is_constructible<T, decltype(ubiq_rref_constructor{I})...>::value) || is_single_field_and_aggregate_initializable<N, T>::value;
    }

    static constexpr bool value =
        std::is_empty<T>::value || std::is_array<T>::value || std::is_fundamental<T>::value || is_not_constructible_n(detail::make_index_sequence<N>{});
};

            #endif  // #ifndef __cpp_lib_is_aggregate

///////////////////// Detect aggregates with inheritance
template<class Derived, class U>
constexpr bool static_assert_non_inherited() noexcept
{
    static_assert(
        !std::is_base_of<U, Derived>::value,
        "====================> Boost.PFR: Boost.PFR: Inherited types are not supported.");
    return true;
}

template<class Derived>
struct ubiq_lref_base_asserting
{
    template<class Type>
    constexpr operator Type&() const&&                                  // tweak for template_unconstrained.cpp like cases
        noexcept(detail::static_assert_non_inherited<Derived, Type>())  // force the computation of assert function
    {
        return detail::unsafe_declval<Type&>();
    }

    template<class Type>
    constexpr operator Type&() const&                                   // tweak for optional_chrono.cpp like cases
        noexcept(detail::static_assert_non_inherited<Derived, Type>())  // force the computation of assert function
    {
        return detail::unsafe_declval<Type&>();
    }
};

template<class Derived>
struct ubiq_rref_base_asserting
{
    template<class Type>
    /*constexpr*/ operator Type() const&&                               // Allows initialization of rvalue reference fields and move-only types
        noexcept(detail::static_assert_non_inherited<Derived, Type>())  // force the computation of assert function
    {
        return detail::unsafe_declval<Type>();
    }
};

template<class T, std::size_t I0, std::size_t... I, class /*Enable*/ = typename std::enable_if<std::is_copy_constructible<T>::value>::type>
constexpr auto assert_first_not_base(std::index_sequence<I0, I...>) noexcept
    -> typename std::add_pointer<decltype(T{ubiq_lref_base_asserting<T>{}, ubiq_lref_constructor{I}...})>::type
{
    return nullptr;
}

template<class T, std::size_t I0, std::size_t... I, class /*Enable*/ = typename std::enable_if<!std::is_copy_constructible<T>::value>::type>
constexpr auto assert_first_not_base(std::index_sequence<I0, I...>) noexcept
    -> typename std::add_pointer<decltype(T{ubiq_rref_base_asserting<T>{}, ubiq_rref_constructor{I}...})>::type
{
    return nullptr;
}

template<class T>
constexpr void* assert_first_not_base(std::index_sequence<>) noexcept
{
    return nullptr;
}

///////////////////// Helper for SFINAE on fields count
template<class T, std::size_t... I, class /*Enable*/ = typename std::enable_if<std::is_copy_constructible<T>::value>::type>
constexpr auto enable_if_constructible_helper(std::index_sequence<I...>) noexcept
    -> typename std::add_pointer<decltype(T{ubiq_lref_constructor{I}...})>::type;

template<class T, std::size_t... I, class /*Enable*/ = typename std::enable_if<!std::is_copy_constructible<T>::value>::type>
constexpr auto enable_if_constructible_helper(std::index_sequence<I...>) noexcept
    -> typename std::add_pointer<decltype(T{ubiq_rref_constructor{I}...})>::type;

template<class T, std::size_t N, class /*Enable*/ = decltype(enable_if_constructible_helper<T>(std::make_index_sequence<N>()))>
using enable_if_constructible_helper_t = std::size_t;

///////////////////// Helpers for range size detection
template<std::size_t Begin, std::size_t Last>
using is_one_element_range = std::integral_constant<bool, Begin == Last>;

using multi_element_range = std::false_type;
using one_element_range = std::true_type;

///////////////////// Non greedy fields count search. Templates instantiation depth is log(sizeof(T)), templates instantiation count is log(sizeof(T)).
template<class T, std::size_t Begin, std::size_t Middle>
constexpr std::size_t detect_fields_count(detail::one_element_range, long) noexcept
{
    static_assert(
        Begin == Middle,
        "====================> Boost.PFR: Internal logic error.");
    return Begin;
}

template<class T, std::size_t Begin, std::size_t Middle>
constexpr std::size_t detect_fields_count(detail::multi_element_range, int) noexcept;

template<class T, std::size_t Begin, std::size_t Middle>
constexpr auto detect_fields_count(detail::multi_element_range, long) noexcept
    -> detail::enable_if_constructible_helper_t<T, Middle>
{
    constexpr std::size_t next_v = Middle + (Middle - Begin + 1) / 2;
    return detail::detect_fields_count<T, Middle, next_v>(detail::is_one_element_range<Middle, next_v>{}, 1L);
}

template<class T, std::size_t Begin, std::size_t Middle>
constexpr std::size_t detect_fields_count(detail::multi_element_range, int) noexcept
{
    constexpr std::size_t next_v = Begin + (Middle - Begin) / 2;
    return detail::detect_fields_count<T, Begin, next_v>(detail::is_one_element_range<Begin, next_v>{}, 1L);
}

///////////////////// Greedy search. Templates instantiation depth is log(sizeof(T)), templates instantiation count is log(sizeof(T))*T in worst case.
template<class T, std::size_t N>
constexpr auto detect_fields_count_greedy_remember(long) noexcept
    -> detail::enable_if_constructible_helper_t<T, N>
{
    return N;
}

template<class T, std::size_t N>
constexpr std::size_t detect_fields_count_greedy_remember(int) noexcept
{
    return 0;
}

template<class T, std::size_t Begin, std::size_t Last>
constexpr std::size_t detect_fields_count_greedy(detail::one_element_range) noexcept
{
    static_assert(
        Begin == Last,
        "====================> Boost.PFR: Internal logic error.");
    return detail::detect_fields_count_greedy_remember<T, Begin>(1L);
}

template<class T, std::size_t Begin, std::size_t Last>
constexpr std::size_t detect_fields_count_greedy(detail::multi_element_range) noexcept
{
    constexpr std::size_t middle = Begin + (Last - Begin) / 2;
    constexpr std::size_t fields_count_big_range = detail::detect_fields_count_greedy<T, middle + 1, Last>(
        detail::is_one_element_range<middle + 1, Last>{});

    constexpr std::size_t small_range_begin = (fields_count_big_range ? 0 : Begin);
    constexpr std::size_t small_range_last = (fields_count_big_range ? 0 : middle);
    constexpr std::size_t fields_count_small_range = detail::detect_fields_count_greedy<T, small_range_begin, small_range_last>(
        detail::is_one_element_range<small_range_begin, small_range_last>{});
    return fields_count_big_range ? fields_count_big_range : fields_count_small_range;
}

///////////////////// Choosing between array size, greedy and non greedy search.
template<class T, std::size_t N>
constexpr auto detect_fields_count_dispatch(size_t_<N>, long, long) noexcept
    -> typename std::enable_if<std::is_array<T>::value, std::size_t>::type
{
    return sizeof(T) / sizeof(typename std::remove_all_extents<T>::type);
}

template<class T, std::size_t N>
constexpr auto detect_fields_count_dispatch(size_t_<N>, long, int) noexcept
    -> decltype(sizeof(T{}))
{
    constexpr std::size_t middle = N / 2 + 1;
    return detail::detect_fields_count<T, 0, middle>(detail::multi_element_range{}, 1L);
}

template<class T, std::size_t N>
constexpr std::size_t detect_fields_count_dispatch(size_t_<N>, int, int) noexcept
{
    // T is not default aggregate initialzable. It means that at least one of the members is not default constructible,
    // so we have to check all the aggregate initializations for T up to N parameters and return the bigest succeeded
    // (we can not use binary search for detecting fields count).
    return detail::detect_fields_count_greedy<T, 0, N>(detail::multi_element_range{});
}

///////////////////// Returns fields count
template<class T>
constexpr std::size_t fields_count() noexcept
{
    using type = std::remove_cv_t<T>;

    static_assert(
        !std::is_reference<type>::value,
        "====================> Boost.PFR: Attempt to get fields count on a reference. This is not allowed because that could hide an issue and different library users expect different behavior in that case.");

            #if !BOOST_PFR_HAS_GUARANTEED_COPY_ELISION
    static_assert(
        std::is_copy_constructible<std::remove_all_extents_t<type>>::value || (std::is_move_constructible<std::remove_all_extents_t<type>>::value && std::is_move_assignable<std::remove_all_extents_t<type>>::value),
        "====================> Boost.PFR: Type and each field in the type must be copy constructible (or move constructible and move assignable).");
            #endif  // #if !BOOST_PFR_HAS_GUARANTEED_COPY_ELISION

    static_assert(
        !std::is_polymorphic<type>::value,
        "====================> Boost.PFR: Type must have no virtual function, because otherwise it is not aggregate initializable.");

            #ifdef __cpp_lib_is_aggregate
    static_assert(
        std::is_aggregate<type>::value  // Does not return `true` for built-in types.
            || std::is_scalar<type>::value,
        "====================> Boost.PFR: Type must be aggregate initializable.");
            #endif

                // Can't use the following. See the non_std_layout.cpp test.
                //#if !BOOST_PFR_USE_CPP17
                //    static_assert(
                //        std::is_standard_layout<type>::value,   // Does not return `true` for structs that have non standard layout members.
                //        "Type must be aggregate initializable."
                //    );
                //#endif

            #if defined(_MSC_VER) && (_MSC_VER <= 1920)
    // Workaround for msvc compilers. Versions <= 1920 have a limit of max 1024 elements in template parameter pack
    constexpr std::size_t max_fields_count = (sizeof(type) * CHAR_BIT >= 1024 ? 1024 : sizeof(type) * CHAR_BIT);
            #else
    constexpr std::size_t max_fields_count = (sizeof(type) * CHAR_BIT);  // We multiply by CHAR_BIT because the type may have bitfields in T
            #endif

    constexpr std::size_t result = detail::detect_fields_count_dispatch<type>(size_t_<max_fields_count>{}, 1L, 1L);

    detail::assert_first_not_base<type>(std::make_index_sequence<result>{});

            #ifndef __cpp_lib_is_aggregate
    static_assert(
        is_aggregate_initializable_n<type, result>::value,
        "====================> Boost.PFR: Types with user specified constructors (non-aggregate initializable types) are not supported.");
            #endif

    static_assert(
        result != 0 || std::is_empty<type>::value || std::is_fundamental<type>::value || std::is_reference<type>::value,
        "====================> Boost.PFR: If there's no other failed static asserts then something went wrong. Please report this issue to the github along with the structure you're reflecting.");

    return result;
}

}  // namespace detail
}  // namespace pfr
}  // namespace boost

            #ifdef __clang__
                #pragma clang diagnostic pop
            #endif

        #endif  // BOOST_PFR_DETAIL_FIELDS_COUNT_HPP

namespace boost {
namespace pfr {
namespace detail {

template<class T>
constexpr auto tie_as_tuple(T& val) noexcept
{
    static_assert(
        !std::is_union<T>::value,
        "====================> Boost.PFR: For safety reasons it is forbidden to reflect unions. See `Reflection of unions` section in the docs for more info.");
    typedef size_t_<boost::pfr::detail::fields_count<T>()> fields_count_tag;
    return boost::pfr::detail::tie_as_tuple(val, fields_count_tag{});
}
}  // namespace detail
}  // namespace pfr
}  // namespace boost

    #endif  // BOOST_PFR_DETAIL_CORE17_HPP

    #include <type_traits>
    #include <utility>  // metaprogramming stuff

/// \file boost/pfr/core.hpp
/// Contains all the basic tuple-like interfaces \forcedlink{get}, \forcedlink{tuple_size}, \forcedlink{tuple_element_t}, and others.
///
/// \b Synopsis:

namespace boost {
namespace pfr {

/// \brief Returns reference or const reference to a field with index `I` in \aggregate `val`.
/// Overload taking the type `U` returns reference or const reference to a field
/// with provided type `U` in \aggregate `val` if there's only one field of such type in `val`.
///
/// \b Example:
/// \code
///     struct my_struct { int i, short s; };
///     my_struct s {10, 11};
///
///     assert(boost::pfr::get<0>(s) == 10);
///     boost::pfr::get<1>(s) = 0;
///
///     assert(boost::pfr::get<int>(s) == 10);
///     boost::pfr::get<short>(s) = 11;
/// \endcode
template<std::size_t I, class T>
constexpr decltype(auto) get(const T& val) noexcept
{
    return detail::sequence_tuple::get<I>(detail::tie_as_tuple(val));
}

/// \overload get
template<std::size_t I, class T>
constexpr decltype(auto) get(T& val
    #if !BOOST_PFR_USE_CPP17
                             ,
                             std::enable_if_t<std::is_assignable<T, T>::value>* = nullptr
    #endif
                             ) noexcept
{
    return detail::sequence_tuple::get<I>(detail::tie_as_tuple(val));
}

    #if !BOOST_PFR_USE_CPP17
/// \overload get
template<std::size_t I, class T>
constexpr auto get(T&, std::enable_if_t<!std::is_assignable<T, T>::value>* = nullptr) noexcept
{
    static_assert(sizeof(T) && false, "====================> Boost.PFR: Calling boost::pfr::get on non const non assignable type is allowed only in C++17");
    return 0;
}
    #endif

/// \overload get
template<std::size_t I, class T>
constexpr auto get(T&& val, std::enable_if_t<std::is_rvalue_reference<T&&>::value>* = nullptr) noexcept
{
    return std::move(detail::sequence_tuple::get<I>(detail::tie_as_tuple(val)));
}

/// \overload get
template<class U, class T>
constexpr const U& get(const T& val) noexcept
{
    return detail::sequence_tuple::get_by_type_impl<const U&>(detail::tie_as_tuple(val));
}

/// \overload get
template<class U, class T>
constexpr U& get(T& val
    #if !BOOST_PFR_USE_CPP17
                 ,
                 std::enable_if_t<std::is_assignable<T, T>::value>* = nullptr
    #endif
                 ) noexcept
{
    return detail::sequence_tuple::get_by_type_impl<U&>(detail::tie_as_tuple(val));
}

    #if !BOOST_PFR_USE_CPP17
/// \overload get
template<class U, class T>
constexpr U& get(T&, std::enable_if_t<!std::is_assignable<T, T>::value>* = nullptr) noexcept
{
    static_assert(sizeof(T) && false, "====================> Boost.PFR: Calling boost::pfr::get on non const non assignable type is allowed only in C++17");
    return 0;
}
    #endif

/// \overload get
template<class U, class T>
constexpr U&& get(T&& val, std::enable_if_t<std::is_rvalue_reference<T&&>::value>* = nullptr) noexcept
{
    return std::move(detail::sequence_tuple::get_by_type_impl<U&>(detail::tie_as_tuple(val)));
}

/// \brief `tuple_element` has a member typedef `type` that returns the type of a field with index I in \aggregate T.
///
/// \b Example:
/// \code
///     std::vector< boost::pfr::tuple_element<0, my_structure>::type > v;
/// \endcode
template<std::size_t I, class T>
using tuple_element = detail::sequence_tuple::tuple_element<I, decltype(::boost::pfr::detail::tie_as_tuple(std::declval<T&>()))>;

/// \brief Type of a field with index `I` in \aggregate `T`.
///
/// \b Example:
/// \code
///     std::vector< boost::pfr::tuple_element_t<0, my_structure> > v;
/// \endcode
template<std::size_t I, class T>
using tuple_element_t = typename tuple_element<I, T>::type;

}  // namespace pfr
}  // namespace boost

#endif  // BOOST_PFR_CORE_HPP

// Copyright (c) 2023 Bela Schaum, X-Ryl669, Denis Mikhailov.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Initial implementation by Bela Schaum, https://github.com/schaumb
// The way to make it union and UB free by X-Ryl669, https://github.com/X-Ryl669
//

#ifndef BOOST_PFR_CORE_NAME_HPP
    #define BOOST_PFR_CORE_NAME_HPP
    #pragma once

    // Copyright (c) 2023 Bela Schaum, X-Ryl669, Denis Mikhailov.
    //
    // Distributed under the Boost Software License, Version 1.0. (See accompanying
    // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    // Initial implementation by Bela Schaum, https://github.com/schaumb
    // The way to make it union and UB free by X-Ryl669, https://github.com/X-Ryl669
    //

    #ifndef BOOST_PFR_DETAIL_CORE_NAME20_STATIC_HPP
        #define BOOST_PFR_DETAIL_CORE_NAME20_STATIC_HPP
        #pragma once

        // Copyright (c) 2023 Bela Schaum, X-Ryl669, Denis Mikhailov.
        //
        // Distributed under the Boost Software License, Version 1.0. (See accompanying
        // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

        // Initial implementation by Bela Schaum, https://github.com/schaumb
        // The way to make it union and UB free by X-Ryl669, https://github.com/X-Ryl669
        //

        #ifndef BOOST_PFR_DETAIL_FAKE_OBJECT_HPP
            #define BOOST_PFR_DETAIL_FAKE_OBJECT_HPP
            #pragma once

namespace boost {
namespace pfr {
namespace detail {

template<class T>
extern const T fake_object;

}
}  // namespace pfr
}  // namespace boost

        #endif  // BOOST_PFR_DETAIL_FAKE_OBJECT_HPP

        #include <array>
        #include <memory>  // for std::addressof
        #include <string_view>
        #include <type_traits>

namespace boost {
namespace pfr {
namespace detail {

struct core_name_skip
{
    std::size_t size_at_begin;
    std::size_t size_at_end;
    bool is_backward;
    std::string_view until_runtime;

    consteval std::string_view apply(std::string_view sv) const noexcept
    {
        // We use std::min here to make the compiler diagnostic shorter and
        // cleaner in case of misconfigured BOOST_PFR_CORE_NAME_PARSING
        sv.remove_prefix((std::min)(size_at_begin, sv.size()));
        sv.remove_suffix((std::min)(size_at_end, sv.size()));
        if (until_runtime.empty())
        {
            return sv;
        }

        const auto found = is_backward ? sv.rfind(until_runtime)
                                       : sv.find(until_runtime);

        const auto cut_until = found + until_runtime.size();
        const auto safe_cut_until = (std::min)(cut_until, sv.size());
        return sv.substr(safe_cut_until);
    }
};

struct backward
{
    explicit consteval backward(std::string_view value) noexcept
      : value(value)
    {}

    std::string_view value;
};

consteval core_name_skip make_core_name_skip(std::size_t size_at_begin,
                                             std::size_t size_at_end,
                                             std::string_view until_runtime) noexcept
{
    return core_name_skip{size_at_begin, size_at_end, false, until_runtime};
}

consteval core_name_skip make_core_name_skip(std::size_t size_at_begin,
                                             std::size_t size_at_end,
                                             backward until_runtime) noexcept
{
    return core_name_skip{size_at_begin, size_at_end, true, until_runtime.value};
}

// it might be compilation failed without this workaround sometimes
// See https://github.com/llvm/llvm-project/issues/41751 for details
template<class>
consteval std::string_view clang_workaround(std::string_view value) noexcept
{
    return value;
}

template<class MsvcWorkaround, auto ptr>
consteval auto name_of_field_impl() noexcept
{
            // Some of the following compiler specific macro may be defined only
            // inside the function body:

        #ifndef BOOST_PFR_FUNCTION_SIGNATURE
            #if defined(__FUNCSIG__)
                #define BOOST_PFR_FUNCTION_SIGNATURE __FUNCSIG__
            #elif defined(__PRETTY_FUNCTION__) || defined(__GNUC__) || defined(__clang__)
                #define BOOST_PFR_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
            #else
                #define BOOST_PFR_FUNCTION_SIGNATURE ""
            #endif
        #endif

    constexpr std::string_view sv = detail::clang_workaround<MsvcWorkaround>(BOOST_PFR_FUNCTION_SIGNATURE);
    static_assert(!sv.empty(),
                  "====================> Boost.PFR: Field reflection parser configured in a wrong way. "
                  "Please define the BOOST_PFR_FUNCTION_SIGNATURE to a compiler specific macro, "
                  "that outputs the whole function signature including non-type template parameters.");

    constexpr auto skip = detail::make_core_name_skip BOOST_PFR_CORE_NAME_PARSING;
    static_assert(skip.size_at_begin + skip.size_at_end + skip.until_runtime.size() < sv.size(),
                  "====================> Boost.PFR: Field reflection parser configured in a wrong way. "
                  "It attempts to skip more chars than available. "
                  "Please define BOOST_PFR_CORE_NAME_PARSING to correct values. See documentation section "
                  "'Limitations and Configuration' for more information.");
    constexpr auto fn = skip.apply(sv);
    static_assert(
        !fn.empty(),
        "====================> Boost.PFR: Extraction of field name is misconfigured for your compiler. "
        "It skipped all the input, leaving the field name empty. "
        "Please define BOOST_PFR_CORE_NAME_PARSING to correct values. See documentation section "
        "'Limitations and Configuration' for more information.");
    auto res = std::array<char, fn.size() + 1>{};

    auto* out = res.data();
    for (auto x : fn)
    {
        *out = x;
        ++out;
    }

    return res;
}

        #ifdef __clang__
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wundefined-var-template"

// clang 16 and earlier don't support address of non-static member as template parameter
// but fortunately it's possible to use C++20 non-type template parameters in another way
// even in clang 16 and more older clangs
// all we need is to wrap pointer into 'clang_wrapper_t' and then pass it into template
template<class T>
struct clang_wrapper_t
{
    T v;
};
template<class T>
clang_wrapper_t(T) -> clang_wrapper_t<T>;

template<class T>
constexpr auto make_clang_wrapper(const T& arg) noexcept
{
    return clang_wrapper_t{arg};
}

        #else

template<class T>
constexpr const T& make_clang_wrapper(const T& arg) noexcept
{
    // It's everything OK with address of non-static member as template parameter support on this compiler
    // so we don't need a wrapper here, just pass the pointer into template
    return arg;
}

        #endif

template<class MsvcWorkaround, auto ptr>
consteval auto name_of_field() noexcept
{
    // Sanity check: known field name must match the deduced one
    static_assert(
        sizeof(MsvcWorkaround)  // do not trigger if `name_of_field()` is not used
            && std::string_view{
                   detail::name_of_field_impl<
                       core_name_skip,
                       detail::make_clang_wrapper(std::addressof(
                           fake_object<core_name_skip>.size_at_begin))>()
                       .data()} == "size_at_begin",
        "====================> Boost.PFR: Extraction of field name is misconfigured for your compiler. "
        "It does not return the proper field name. "
        "Please define BOOST_PFR_CORE_NAME_PARSING to correct values. See documentation section "
        "'Limitations and Configuration' for more information.");

    return detail::name_of_field_impl<MsvcWorkaround, ptr>();
}

// Storing part of a string literal into an array minimizes the binary size.
//
// Without passing 'T' into 'name_of_field' different fields from different structures might have the same name!
// See https://developercommunity.visualstudio.com/t/__FUNCSIG__-outputs-wrong-value-with-C/10458554 for details
template<class T, std::size_t I>
inline constexpr auto stored_name_of_field = detail::name_of_field<T,
                                                                   detail::make_clang_wrapper(std::addressof(detail::sequence_tuple::get<I>(
                                                                       detail::tie_as_tuple(detail::fake_object<T>))))>();

        #ifdef __clang__
            #pragma clang diagnostic pop
        #endif

template<class T, std::size_t... I>
constexpr auto tie_as_names_tuple_impl(std::index_sequence<I...>) noexcept
{
    return detail::sequence_tuple::make_sequence_tuple(std::string_view{stored_name_of_field<T, I>.data()}...);
}

template<class T, std::size_t I>
constexpr std::string_view get_name() noexcept
{
    static_assert(
        !std::is_union<T>::value,
        "====================> Boost.PFR: For safety reasons it is forbidden to reflect unions. See `Reflection of unions` section in the docs for more info.");
    static_assert(
        !std::is_array<T>::value,
        "====================> Boost.PFR: It is impossible to extract name from old C array since it doesn't have named members");
    static_assert(
        sizeof(T) && BOOST_PFR_USE_CPP17,
        "====================> Boost.PFR: Extraction of field's names is allowed only when the BOOST_PFR_USE_CPP17 macro enabled.");

    return stored_name_of_field<T, I>.data();
}

}  // namespace detail
}  // namespace pfr
}  // namespace boost

    #endif  // BOOST_PFR_DETAIL_CORE_NAME20_STATIC_HPP

    #include <cstddef>  // for std::size_t

    // Copyright (c) 2016-2023 Antony Polukhin
    //
    // Distributed under the Boost Software License, Version 1.0. (See accompanying
    // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PFR_TUPLE_SIZE_HPP
        #define BOOST_PFR_TUPLE_SIZE_HPP
        #pragma once

        #include <type_traits>
        #include <utility>  // metaprogramming stuff

/// \file boost/pfr/tuple_size.hpp
/// Contains tuple-like interfaces to get fields count \forcedlink{tuple_size}, \forcedlink{tuple_size_v}.
///
/// \b Synopsis:
namespace boost {
namespace pfr {

/// Has a static const member variable `value` that contains fields count in a T.
/// Works for any T that satisfies \aggregate.
///
/// \b Example:
/// \code
///     std::array<int, boost::pfr::tuple_size<my_structure>::value > a;
/// \endcode
template<class T>
using tuple_size = detail::size_t_<boost::pfr::detail::fields_count<T>()>;

/// `tuple_size_v` is a template variable that contains fields count in a T and
/// works for any T that satisfies \aggregate.
///
/// \b Example:
/// \code
///     std::array<int, boost::pfr::tuple_size_v<my_structure> > a;
/// \endcode
template<class T>
constexpr std::size_t tuple_size_v = tuple_size<T>::value;

}  // namespace pfr
}  // namespace boost

    #endif  // BOOST_PFR_TUPLE_SIZE_HPP

/// \file boost/pfr/core_name.hpp
/// Contains functions \forcedlink{get_name} and \forcedlink{names_as_array} to know which names each field of any \aggregate has.
///
/// \fnrefl for details.
///
/// \b Synopsis:

namespace boost {
namespace pfr {

/// \brief Returns name of a field with index `I` in \aggregate `T`.
///
/// \b Example:
/// \code
///     struct my_struct { int i, short s; };
///
///     assert(boost::pfr::get_name<0, my_struct>() == "i");
///     assert(boost::pfr::get_name<1, my_struct>() == "s");
/// \endcode
template<std::size_t I, class T>
constexpr
    #ifdef BOOST_PFR_DOXYGEN_INVOKED
    std::string_view
    #else
    auto
    #endif
    get_name() noexcept
{
    return detail::get_name<T, I>();
}

}  // namespace pfr
}  // namespace boost

#endif  // BOOST_PFR_CORE_NAME_HPP

// Copyright (c) 2022 Denis Mikhailov
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PFR_TRAITS_HPP
    #define BOOST_PFR_TRAITS_HPP
    #pragma once

    // Copyright (c) 2022 Denis Mikhailov
    //
    // Distributed under the Boost Software License, Version 1.0. (See accompanying
    // file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    #ifndef BOOST_PFR_DETAIL_POSSIBLE_REFLECTABLE_HPP
        #define BOOST_PFR_DETAIL_POSSIBLE_REFLECTABLE_HPP
        #pragma once

        #include <type_traits>  // for std::is_aggregate

namespace boost {
namespace pfr {
namespace detail {

///////////////////// Returns false when the type exactly wasn't be reflectable
template<class T, class WhatFor>
constexpr decltype(is_reflectable<T, WhatFor>::value) possible_reflectable(long) noexcept
{
    return is_reflectable<T, WhatFor>::value;
}

        #if BOOST_PFR_ENABLE_IMPLICIT_REFLECTION

template<class T, class WhatFor>
constexpr bool possible_reflectable(int) noexcept
{
            #if defined(__cpp_lib_is_aggregate)
    using type = std::remove_cv_t<T>;
    return std::is_aggregate<type>();
            #else
    return true;
            #endif
}

        #else

template<class T, class WhatFor>
constexpr bool possible_reflectable(int) noexcept
{
    // negative answer here won't change behaviour in PFR-dependent libraries(like Fusion)
    return false;
}

        #endif

}  // namespace detail
}  // namespace pfr
}  // namespace boost

    #endif  // BOOST_PFR_DETAIL_POSSIBLE_REFLECTABLE_HPP

    #include <type_traits>

/// \file boost/pfr/traits.hpp
/// Contains traits \forcedlink{is_reflectable} and \forcedlink{is_implicitly_reflectable} for detecting an ability to reflect type.
///
/// \b Synopsis:

namespace boost {
namespace pfr {

/// Has a static const member variable `value` when it is known that type T can or can't be reflected using Boost.PFR; otherwise, there is no member variable.
/// Every user may (and in some difficult cases - should) specialize is_reflectable on his own.
///
/// \b Example:
/// \code
///     namespace boost { namespace pfr {
///         template<class All> struct is_reflectable<A, All> : std::false_type {};       // 'A' won't be interpreted as reflectable everywhere
///         template<> struct is_reflectable<B, boost_fusion_tag> : std::false_type {};   // 'B' won't be interpreted as reflectable in only Boost Fusion
///     }}
/// \endcode
/// \note is_reflectable affects is_implicitly_reflectable, the decision made by is_reflectable is used by is_implicitly_reflectable.
template<class T, class WhatFor>
struct is_reflectable
{ /*  does not have 'value' because value is unknown */
};

// these specs can't be inherited from 'std::integral_constant< bool, boost::pfr::is_reflectable<T, WhatFor>::value >',
// because it will break the sfinae-friendliness
template<class T, class WhatFor>
struct is_reflectable<const T, WhatFor> : boost::pfr::is_reflectable<T, WhatFor>
{};

template<class T, class WhatFor>
struct is_reflectable<volatile T, WhatFor> : boost::pfr::is_reflectable<T, WhatFor>
{};

template<class T, class WhatFor>
struct is_reflectable<const volatile T, WhatFor> : boost::pfr::is_reflectable<T, WhatFor>
{};

/// Checks the input type for the potential to be reflected.
/// Specialize is_reflectable if you disagree with is_implicitly_reflectable's default decision.
template<class T, class WhatFor>
using is_implicitly_reflectable = std::integral_constant<bool, boost::pfr::detail::possible_reflectable<T, WhatFor>(1L)>;

/// Checks the input type for the potential to be reflected.
/// Specialize is_reflectable if you disagree with is_implicitly_reflectable_v's default decision.
template<class T, class WhatFor>
constexpr bool is_implicitly_reflectable_v = is_implicitly_reflectable<T, WhatFor>::value;

}  // namespace pfr
}  // namespace boost

#endif  // BOOST_PFR_TRAITS_HPP

#endif  //  OOST_PFR_HPP
