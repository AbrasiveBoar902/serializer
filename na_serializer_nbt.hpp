#include "na_serializer.hpp"
#include "pfr_used.hpp"
#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace na::nbt {
struct nbt
{};

template<::std::endian nbt_endian = ::std::endian::big>
struct option : na::serializer::serialize_option
{
    constexpr static auto endian = nbt_endian;
};

template<typename T>
struct is_option : ::std::false_type
{};
template<::std::endian E>
struct is_option<option<E>> : ::std::true_type
{};

template<typename T>
concept any_option = is_option<T>::value;

enum class nbt_error
{
    ok,
    end_of_file,
    invalid
};

enum nbt_tag_type : ::std::uint8_t
{
    tag_end = 0,
    tag_byte = 1,
    tag_short = 2,
    tag_int = 3,
    tag_long = 4,
    tag_float = 5,
    tag_double = 6,
    tag_byte_array = 7,
    tag_string = 8,
    tag_list = 9,
    tag_compound = 10,
    tag_int_array = 11,
    tag_long_array = 12,

    tag_uncompatible = 255
};

namespace detail {
template<typename T>
inline consteval nbt_tag_type nbt_type_id_impl() noexcept;
}
template<typename T>
constexpr nbt_tag_type nbt_type_id = detail::nbt_type_id_impl<T>();
template<typename T>
concept any_nbt = (nbt_type_id<T> != nbt_tag_type::tag_uncompatible);

// nbt types
using nbt_byte = ::std::int8_t;
using nbt_short = ::std::int16_t;
using nbt_int = ::std::int32_t;
using nbt_long = ::std::int64_t;
using nbt_float = float;
using nbt_double = double;
template<::std::size_t N>
using nbt_byte_array = ::std::array<nbt_byte, N>;
using nbt_string = ::std::u8string_view;
template<any_nbt Type, ::std::size_t N>
struct nbt_list : ::std::array<Type, N>
{
    inline static constexpr ::std::size_t nbt_list_length{N};
    using type = Type;
};
// nbt_compound is a struct
template<::std::size_t N>
using nbt_int_array = ::std::array<nbt_int, N>;
template<::std::size_t N>
using nbt_long_array = ::std::array<nbt_long, N>;

namespace detail {
template<typename T>
inline constexpr bool is_nbt_list_v = false;
template<any_nbt T, ::std::size_t N>
inline constexpr bool is_nbt_list_v<nbt_list<T, N>> = true;

template<typename T>
inline constexpr bool is_nbt_byte_array_v = false;
template<::std::size_t N>
inline constexpr bool is_nbt_byte_array_v<nbt_byte_array<N>> = true;

template<typename T>
inline constexpr bool is_nbt_int_array_v = false;
template<::std::size_t N>
inline constexpr bool is_nbt_int_array_v<nbt_int_array<N>> = true;

template<typename T>
inline constexpr bool is_nbt_long_array_v = false;
template<::std::size_t N>
inline constexpr bool is_nbt_long_array_v<nbt_long_array<N>> = true;
}  // namespace detail

template<typename T>
concept any_nbt_list = detail::is_nbt_list_v<T>;
template<typename T>
concept any_nbt_byte_array = detail::is_nbt_byte_array_v<T>;
template<typename T>
concept any_nbt_int_array = detail::is_nbt_int_array_v<T>;
template<typename T>
concept any_nbt_long_array = detail::is_nbt_long_array_v<T>;
template<typename T>
concept any_nbt_array = any_nbt_byte_array<T> || any_nbt_int_array<T> || any_nbt_long_array<T>;

template<typename T>
concept any_nbt_compound = boost::pfr::is_implicitly_reflectable_v<T, nbt> && !any_nbt_list<T>;
namespace detail {
template<typename T>
consteval nbt_tag_type nbt_type_id_impl() noexcept
{
    if constexpr (std::same_as<T, nbt_byte>)
    {
        return nbt_tag_type::tag_byte;
    }
    else if constexpr (std::same_as<T, nbt_short>)
    {
        return nbt_tag_type::tag_short;
    }
    else if constexpr (std::same_as<T, nbt_int>)
    {
        return nbt_tag_type::tag_int;
    }
    else if constexpr (std::same_as<T, nbt_long>)
    {
        return nbt_tag_type::tag_long;
    }
    else if constexpr (std::same_as<T, nbt_float>)
    {
        return nbt_tag_type::tag_float;
    }
    else if constexpr (std::same_as<T, nbt_double>)
    {
        return nbt_tag_type::tag_double;
    }
    else if constexpr (any_nbt_byte_array<T>)
    {
        return nbt_tag_type::tag_byte_array;
    }
    else if constexpr (std::same_as<T, nbt_string>)
    {
        return nbt_tag_type::tag_string;
    }
    else if constexpr (any_nbt_list<T>)
    {
        return nbt_tag_type::tag_list;
    }
    else if constexpr (any_nbt_compound<T>)
    {
        return nbt_tag_type::tag_compound;
    }
    else if constexpr (any_nbt_int_array<T>)
    {
        return nbt_tag_type::tag_int_array;
    }
    else if constexpr (any_nbt_long_array<T>)
    {
        return nbt_tag_type::tag_long_array;
    }
    else
    {
        return nbt_tag_type::tag_uncompatible;
    }
}
}  // namespace detail

namespace detail {
template<any_nbt T>
inline consteval bool is_simple_nbt_impl()
{
    if constexpr (::std::is_arithmetic_v<T> || any_nbt_array<T> || ::std::same_as<T, nbt_string>)
    {
        return true;
    }
    else if constexpr (any_nbt_list<T>)
    {
        using U = T::value_type;
        return is_simple_nbt_impl<U>();
    }
    else
    {
        return false;
    }
}
}  // namespace detail

template<typename T>
concept any_simple_nbt = any_nbt<T> && detail::is_simple_nbt_impl<T>();

template<typename T>
concept any_complex_nbt = any_nbt<T> && !any_simple_nbt<T>;

template<typename T>
concept any_simple_nbt_list = any_nbt_list<T> && any_simple_nbt<T>;

template<typename T>
concept any_complex_nbt_list = any_nbt_list<T> && !any_simple_nbt_list<T>;

template<typename T, std::endian nbt_endian>
requires(nbt_endian == std::endian::big || nbt_endian == ::std::endian::little) inline constexpr T const endian_get(::std::byte const* current_pos) noexcept
{
    if constexpr (nbt_endian == std::endian::native)
    {
        return *reinterpret_cast<const T*>(current_pos);
    }
    else
    {
        if constexpr (::std::integral<T>)
            return ::std::byteswap(*reinterpret_cast<const T*>(current_pos));
        else if constexpr (::std::floating_point<T>)
        {
            if constexpr (::std::same_as<T, float>)
            {
                return ::std::bit_cast<T>(::std::byteswap(*reinterpret_cast<::std::uint32_t const*>(current_pos)));
            }
            else if constexpr (::std::same_as<T, double>)
                return ::std::bit_cast<T>(::std::byteswap(*reinterpret_cast<::std::uint64_t const*>(current_pos)));
            else
                static_assert(::std::same_as<T, float> || ::std::same_as<T, double>);
        }
        else
        {
            static_assert(::std::integral<T> || ::std::floating_point<T>);
        }
    }
}
}  // namespace na::nbt

template<na::serializer::any_serialize_option Option, na::serializer::any_serializer_profile Profile, na::serializer::any_node_path Path>
struct na::serializer::node<na::nbt::nbt, Option, Profile, Path>
  : na::serializer::node_base<na::nbt::nbt, Option, Profile, Path>
{
    using this_type = na::serializer::node<na::nbt::nbt, Option, Profile, Path>;

    struct type_helper
    {
        template<typename Type, size_type Index>
        inline static consteval auto type_impl_at()
        {
            static_assert(Index <= Path::size);
            if constexpr (Index == Path::size)
            {
                return Type{};
            }
            else
            {
                using Now = Path::template at<Index>;
                if constexpr (Now::has_guide)
                {
                    return type_impl_at<typename Now::type_guide, Index + 1>();
                }
                else
                {
                    constexpr size_type I0{Now::index};
                    if constexpr (na::nbt::any_nbt_compound<Type>)
                    {
                        static_assert(I0 < boost::pfr::tuple_size_v<Type>, "nbt_index out of range");
                        using U = boost::pfr::tuple_element_t<I0, Type>;
                        return type_impl_at<U, Index + 1>();
                    }
                    else
                    {
                        static_assert(I0 < Type::nbt_list_length, "nbt_index out of range");
                        return type_impl_at<typename Type::value_type, Index + 1>();
                    }
                }
            }
        }

        inline static consteval auto type_impl()
        {
            return type_impl_at<typename Path::root, 0>();
        }
    };

    using type = decltype(type_helper::type_impl());

    constexpr static bool serializable = na::nbt::any_nbt<type>;

    constexpr static bool composite = na::nbt::any_complex_nbt<type>;

    constexpr static bool primitive = na::nbt::any_simple_nbt<type>;

    struct size_helper
    {
        inline static consteval size_type size_impl()
        {
            using T = type;
            if constexpr (na::nbt::any_nbt_compound<T>)
                return boost::pfr::tuple_size_v<T>;
            else if constexpr (na::nbt::any_complex_nbt_list<T>)
                return T::nbt_list_length;
            else
                return 0;
        }
    };

    constexpr static size_type size = size_helper::size_impl();

    struct data_helper
    {
        template<typename T>
        inline static consteval bool payload_memory_compatible_impl()
        {
            if constexpr (na::nbt::any_complex_nbt<T>)
            {
                return false;  // not used
            }
            if constexpr (Option::endian == ::std::endian::native)
            {
                if constexpr (::std::is_arithmetic_v<T> || na::nbt::any_nbt_array<T>)
                    return true;
                else if constexpr (na::nbt::any_nbt_list<T> && payload_memory_compatible_impl<typename T::type>())
                    return true;
                else
                    return false;
            }
            else
            {
                return false;
            }
        }

        inline constexpr static bool prepend_fixed_size()
        {
            return true;
        }

        template<typename T>
        inline constexpr static bool payload_fixed_size()
        {
            if constexpr (na::nbt::any_complex_nbt<T>)
            {
                return false;  // not used
            }
            if constexpr (::std::same_as<T, na::nbt::nbt_string>)
            {
                return false;
            }
            else if constexpr (na::nbt::any_nbt_list<T>)
            {
                return payload_fixed_size<typename T::type>();
            }
            else
            {
                return true;
            }
        }

        inline constexpr static bool postpend_fixed_size()
        {
            return true;
        }

        inline constexpr static size_type prepend_minimal_size()
        {
            using Type = type;
            if constexpr (Path::size == 0)
            {
                if constexpr (na::nbt::any_nbt_list<Type>)
                    return 1 + 2 + 1 + 4;
                else if constexpr (na::nbt::any_nbt_array<Type>)
                    return 1 + 2 + 4;
                else
                    return 1 + 2;
            }
            else
            {
                using ParentType = node_parent<this_type>::type;
                constexpr auto Index = path_last<Path>::index;
                if constexpr (na::nbt::any_nbt_compound<ParentType>)
                {
                    if constexpr (na::nbt::any_nbt_list<Type>)
                        return static_cast<::std::size_t>(1) + 2 + boost::pfr::get_name<Index, ParentType>().size() + 1 + 4;
                    else if constexpr (na::nbt::any_nbt_array<Type>)
                        return static_cast<::std::size_t>(1) + 2 + boost::pfr::get_name<Index, ParentType>().size() + 4;
                    else
                        return static_cast<::std::size_t>(1) + 2 + boost::pfr::get_name<Index, ParentType>().size();
                }
                else  // list
                {
                    static_assert(na::nbt::any_complex_nbt_list<ParentType>);
                    if constexpr (na::nbt::any_nbt_list<Type>)
                        return 1 + 4;
                    else if constexpr (na::nbt::any_nbt_array<Type>)
                        return 4;
                    else
                        return 0;
                }
            }
        }

        template<typename T>
        inline constexpr static size_type payload_minimal_size()
        {
            if constexpr (na::nbt::any_complex_nbt<T>)
            {
                return 0;  // not used
            }
            if constexpr (std::integral<T> || std::floating_point<T>)
            {
                return sizeof(T);
            }
            else if constexpr (na::nbt::any_nbt_array<T>)
            {
                using U = T::value_type;
                return ::std::tuple_size_v<T> * sizeof(U);
            }
            else if constexpr (std::same_as<T, na::nbt::nbt_string>)
            {
                return 2 /* name length */;
            }
            else if constexpr (na::nbt::any_nbt_list<T>)
            {
                return T::nbt_list_length * payload_minimal_size<typename T::type>();
            }
        }

        inline constexpr static size_type postpend_minimal_size()
        {
            if constexpr (na::nbt::any_nbt_compound<type>)
                return 1;
            else
                return 0;
        }
    };

    constexpr static bool payload_memory_compatible = data_helper::template payload_memory_compatible_impl<type>();

    constexpr static bool prepend_fixed_size = data_helper::prepend_fixed_size();

    constexpr static bool payload_fixed_size = data_helper::template payload_fixed_size<type>();

    constexpr static bool postpend_fixed_size = data_helper::postpend_fixed_size();

    constexpr static size_type prepend_minimal_size = data_helper::prepend_minimal_size();

    constexpr static size_type payload_minimal_size = data_helper::template payload_minimal_size<type>();

    constexpr static size_type postpend_minimal_size = data_helper::postpend_minimal_size();

    struct payload_reference_helper
    {
        template<na::nbt::any_complex_nbt T, typename E, any_node_index I0, any_node_index... Is>
        inline constexpr static auto& payload_reference_impl(T& value, node_path<E, I0, Is...>)
        {
            constexpr size_type index{I0::index};
            if constexpr (sizeof...(Is) == 0)
            {
                if constexpr (na::nbt::any_nbt_compound<T>)
                {
                    static_assert(index < boost::pfr::tuple_size_v<T>, "nbt_index out of range");
                    return boost::pfr::get<index>(value);
                }
                else
                {
                    static_assert(index < T::nbt_list_length, "nbt_index out of range");
                    return value[index];
                }
            }
            else
            {
                if constexpr (na::nbt::any_nbt_compound<T>)
                {
                    static_assert(index < boost::pfr::tuple_size_v<T>, "nbt_index out of range");
                    using NextType = boost::pfr::tuple_element_t<index, T>;
                    static_assert(na::nbt::any_complex_nbt<NextType>, "nbt_index_path too long");
                    return payload_reference_impl<NextType>(boost::pfr::get<index>(value), node_path<E, Is...>{});
                }
                else
                {
                    static_assert(index < T::nbt_list_length, "nbt_index out of range");
                    using NextType = T::value_type;
                    static_assert(na::nbt::any_complex_nbt<NextType>, "nbt_index_path too long");
                    return payload_reference_impl<NextType>(value[index], node_path<E, Is...>{});
                }
            }
        }
    };

    inline constexpr static auto& payload_reference(auto& value)
    {
        if constexpr (Path::size == 0)
        {
            return value;
        }
        else
        {
            return payload_reference_helper::template payload_reference_impl<typename Path::root>(value, Path{});
        }
    }

    // deserialize

    using deserialize_context = no_context;

    inline constexpr static bool deserialize_prepend(auto& value, ::std::byte const* start, ::std::size_t& offset, ::std::size_t reversed, deserialize_context& context, na::nbt::nbt_error& error_code)
    {
        return true;
    }

    inline constexpr static bool deserialize_postpend(auto& value, ::std::byte const* start, ::std::size_t& offset, ::std::size_t reversed, deserialize_context& context, na::nbt::nbt_error& error_code)
    {
        return true;
    }

    inline constexpr static bool deserialize_all(auto& value, ::std::byte const* start, ::std::size_t& offset, ::std::size_t reversed, na::nbt::nbt_error& error_code)
    {
        static_assert(::std::same_as<::std::remove_reference_t<decltype(value)>, typename Path::root>);
        constexpr auto minimal_offset{payload_minimal_offset<this_type>};
        auto& ref{payload_reference(value)};
        using T = type;

        if constexpr (na::nbt::any_complex_nbt<T>)
        {
            return true;
        }
        else if constexpr (std::integral<T> || std::floating_point<T>)
        {
            ref = na::nbt::endian_get<T, Option::endian>(start + minimal_offset + offset);
        }
        else if constexpr (na::nbt::any_nbt_array<T>)
        {
            constexpr auto len{::std::tuple_size_v<T>};
            using V = T::value_type;
            auto current_pos{start + minimal_offset + offset};
            for (::std::int32_t _index = 0; _index < len; _index++)
            {
                ref[_index] = na::nbt::endian_get<V, Option::endian>(current_pos);
                current_pos += sizeof(V);
            }
        }
        else if constexpr (na::nbt::any_simple_nbt_list<T>)
        {
            constexpr auto len{T::nbt_list_length};
            using V = T::value_type;
            if constexpr (std::same_as<V, na::nbt::nbt_string>)
            {
                for (::std::int32_t _index = 0; _index < len; _index++)
                {
                    auto current_pos{start + minimal_offset + offset};
                    auto len{na::nbt::endian_get<::std::uint16_t, Option::endian>(current_pos)};
                    offset += len;
                    if (offset > reversed)
                    {
                        return false;
                    }
                    ref[_index] = ::std::u8string_view(reinterpret_cast<char8_t const*>(current_pos + sizeof(::std::uint16_t)), len);
                }
            }
            else
            {
                auto current_pos{start + minimal_offset + offset};
                for (::std::int32_t _index = 0; _index < len; _index++)
                {
                    ref[_index] = na::nbt::endian_get<V, Option::endian>(current_pos);
                    current_pos += sizeof(V);
                }
            }
        }
        else if constexpr (std::same_as<T, na::nbt::nbt_string>)
        {
            auto current_pos{start + minimal_offset + offset};
            auto len{na::nbt::endian_get<::std::uint16_t, Option::endian>(current_pos)};
            offset += len;
            if (offset > reversed)
            {
                return false;
            }
            ref = ::std::u8string_view(reinterpret_cast<char8_t const*>(current_pos + sizeof(::std::uint16_t)), len);
        }
        return true;
    }

    // serialize

    inline constexpr static size_type prepend_extra_size(auto& value)
    {
        return 0;
    }

    inline constexpr static size_type payload_extra_size(auto& value)
    {
        if constexpr (nbt::any_complex_nbt<type>)
            return 0;  // not used
        else if constexpr (payload_fixed_size)
            return 0;
        else if constexpr (::std::same_as<type, nbt::nbt_string>)
        {
            return payload_reference(value).size();
        }
        else
        {
            static_assert(nbt::any_nbt_list<type>);
            // 仅可能为 list...<string>
            auto get_list_extra_size{
                [](auto& value, auto& self) -> size_type {
                    using Type = ::std::remove_cvref_t<decltype(value)>;
                    if constexpr (nbt::any_nbt_list<Type>)
                    {
                        size_type result{0};
                        for (auto& e : value)
                        {
                            result += self(e, self);
                        }
                    }
                    else
                    {
                        static_assert(::std::same_as<Type, nbt::nbt_string>);
                        return value.size();
                    }
                }};
            return get_list_extra_size(value, get_list_extra_size);
        }
    }

    inline constexpr static size_type postpend_extra_size(auto& value)
    {
        return 0;
    }

    using serialize_context = no_context;

    inline constexpr static void serialize_prepend(auto& value, ::std::byte const* start, serialize_context& context)
    {
        using Type = type;
        if constexpr (Path::size == 0)
        {
            if constexpr (na::nbt::any_nbt_list<Type>)
            {
            }
            // return 1 + 2 + 1 + 4;
            else if constexpr (na::nbt::any_nbt_array<Type>)
            {
            }
            // return 1 + 2 + 4;
            else
            {
            }
            // return 1 + 2;
        }
        else
        {
            using ParentType = node_parent<this_type>::type;
            constexpr auto Index = path_last<Path>::index;
            if constexpr (na::nbt::any_nbt_compound<ParentType>)
            {
                if constexpr (na::nbt::any_nbt_list<Type>)
                {
                }
                // return static_cast<::std::size_t>(1) + 2 + boost::pfr::get_name<Index, ParentType>().size() + 1 + 4;
                else if constexpr (na::nbt::any_nbt_array<Type>)
                {
                }
                // return static_cast<::std::size_t>(1) + 2 + boost::pfr::get_name<Index, ParentType>().size() + 4;
                else
                {
                }
                // return static_cast<::std::size_t>(1) + 2 + boost::pfr::get_name<Index, ParentType>().size();
            }
            else  // list
            {
                static_assert(na::nbt::any_complex_nbt_list<ParentType>);
                if constexpr (na::nbt::any_nbt_list<Type>)
                {
                }
                // return 1 + 4;
                else if constexpr (na::nbt::any_nbt_array<Type>)
                {
                }
                // return 4;
                else
                {
                }
                // return 0;
            }
        }
    }
    inline constexpr static void serialize_postpend(auto& value, ::std::byte const* start, serialize_context& context)
    {
    }
    inline constexpr static void serialize_all(auto& value, ::std::byte const* start)
    {
    }
};