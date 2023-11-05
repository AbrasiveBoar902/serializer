#pragma once
#include <concepts>
#include <cstddef>
#include <cstring>
#include <span>
#include <tuple>
#include <type_traits>
namespace na::serializer {
#ifdef __INTELLISENSE__
    #define NA_SERIALIZER_HIDE_IMPL
#endif

template<typename T>
concept any_serializer_tag = true;

template<typename T>
concept without_cvr = !::std::is_const_v<T> && !::std::is_volatile_v<T> && !::std::is_reference_v<T>;

//
// node_index traits fwd
//

template<typename T>
struct is_node_index;

template<typename T>
concept any_node_index = is_node_index<T>::value;

//
// node_path traits fwd
//

template<typename T>
struct is_node_path;

template<typename T>
concept any_node_path = is_node_path<T>::value;

//
// serialize_option traits fwd
//

struct serialize_option_base
{};

template<typename T>
concept any_serialize_option = ::std::is_base_of_v<serialize_option_base, T>;

//
// serializer_profile traits fwd
//

struct serializer_profile_base
{};

template<typename T>
concept any_serializer_profile = ::std::is_base_of_v<serializer_profile_base, T>;

//
// node traits fwd
//

template<typename T>
struct is_node;

template<typename T>
concept any_node = is_node<T>::value;

//
// size_type
//

using size_type = ::std::size_t;
using ssize_type = ::std::make_signed_t<size_type>;

//
// node_index decl
//

struct no_guide
{};

template<size_type Index, typename TypeGuide = no_guide>
struct node_index
{
    constexpr static size_type index = Index;
    using type_guide = TypeGuide;

    constexpr static bool has_guide = !(::std::same_as<TypeGuide, no_guide>);

    template<typename T>
    using with_guide = node_index<Index, T>;
};

//
// node_index traits
//

template<typename T>
struct is_node_index : ::std::false_type
{};

template<size_type Index, typename TypeGuide>
struct is_node_index<node_index<Index, TypeGuide>> : ::std::true_type
{};

//
// node_path decl
//

template<typename Root, any_node_index... Indices>
struct node_path
{
    using root = Root;
    using indices = ::std::tuple<Indices...>;

    constexpr static size_type size = sizeof...(Indices);

    constexpr static ssize_type ssize = sizeof...(Indices);

    template<size_type I>
    requires(I < size) using at = ::std::tuple_element_t<I, indices>;

    template<any_node_index... Is>
    using append = node_path<Root, Indices..., Is...>;

    template<any_node_index... Is>
    using prepend = node_path<Root, Is..., Indices...>;

    template<typename T, T... Is>
    inline static consteval auto take_first_n_helper(std::integer_sequence<T, Is...>)
        -> node_path<Root, at<Is>...>;

    template<size_type I>
    requires(I <= size) using take_first_n = decltype(take_first_n_helper(::std::make_index_sequence<I>{}));
};

template<any_node_path Path>
using path_last = Path::template at<Path::size - 1>;

//
// node_path traits
//

template<typename T>
struct is_node_path : ::std::false_type
{};

template<typename Root, any_node_index... Indices>
struct is_node_path<node_path<Root, Indices...>> : ::std::true_type
{};

//
// serialize_option decl
//

struct serialize_option : serialize_option_base
{};

//
// serializer_profile decl
//

enum class operations
{
    serialize,
    deserialize
};

template<operations Operation>
struct serializer_profile : serializer_profile_base
{
    constexpr static operations operation = Operation;
};

//
// node decl
//

template<any_serializer_tag S, any_serialize_option Option, any_serializer_profile Profile, any_node_path Path>
struct node;

template<any_serializer_tag S, any_serialize_option Option, any_serializer_profile Profile, any_node_path Path>
struct node_base
{
    using serializer_tag = S;
    using option = Option;
    using profile = Profile;
    using path = Path;

    template<any_node_index... Is>
    using at = node<S, Option, Profile, typename Path::template append<Is...>>;
};

template<any_serializer_tag S, any_serialize_option Option, any_serializer_profile Profile, any_node_path Path>
struct node : node_base<S, Option, Profile, Path>
{
    /// <summary>
    /// 一个任意的 primitive_node 在序列化和反序列化中占且仅占一次处理，是序列化/反序列化处理的最小单元。
    /// 有些 primitive_node 是运行时动态展开的，这时候就需要递归调用 serialize/deserialize
    /// </summary>
    // constexpr static bool primitive = ;

    /// <summary>
    /// 一个任意的 composite_node 在序列化和反序列化时都不能被直接处理，而是分别处理它的所有元素。如果元素也为 composite_node 则继续递归。
    /// </summary>
    // constexpr static bool composite = ;

    // using type = ;

    // constexpr static size_type size = ;

    // constexpr static bool payload_memory_compatible = ;

    // constexpr static bool prepend_fixed_size = ;

    // constexpr static bool payload_fixed_size = ;

    // constexpr static bool postpend_fixed_size = ;

    // constexpr static size_type prepend_minimal_size = ;

    // constexpr static size_type payload_minimal_size = ;

    // constexpr static size_type postpend_minimal_size = ;

    // inline constexpr static auto& payload_reference(auto& value)

    // using context = ;

    // inline constexpr static bool deserialize_prepend(auto& value, ::std::byte const* start, ::std::size_t& offset, ::std::size_t reversed, auto& context, auto& error_code)

    // inline constexpr static bool deserialize_postpend(auto& value, ::std::byte const* start, ::std::size_t& offset, ::std::size_t reversed, auto& context, auto& error_code)

    // inline constexpr static bool deserialize_all(auto& value, ::std::byte const* start, ::std::size_t& offset, ::std::size_t reversed, auto& error_code)
};

#ifndef NA_SERIALIZER_HIDE_IMPL
struct flat_size_helper
{
    template<any_node Node>
    inline consteval static size_type flat_size()
    {
        if constexpr (Node::composite)
            return []<typename Ti, Ti... Is>(::std::integer_sequence<Ti, Is...>) {
                return (Node::template at<node_index<Is>>::flat_size + ...);
            }(::std::make_index_sequence<Node::size>{}) +
                   1;
        else
            return 1;
    }
};
#endif  // NA_SERIALIZER_HIDE_IMPL

template<any_node Node>
constexpr static size_type flat_size =
#ifndef NA_SERIALIZER_HIDE_IMPL
    flat_size_helper::flat_size<Node>();
#else
    1;
#endif

struct flat_next_n_helper
{
    template<any_node Node, node_index GetIndexInParent = 0>
    inline consteval static auto next_node();
    template<any_node Node, size_type I>
    inline consteval static auto flat_next_n()
    {
        if constexpr (I == 0)
            return ::std::declval<Node>();
        else
            return flat_next_n<decltype(next_node<Node>()), I - 1>();
    }
};

template<any_node Node, size_type I>
using flat_next_n = decltype(flat_next_n_helper::template flat_next_n<Node, I>());

template<any_node Node>
requires(Node::path::size > 0) using node_parent = node<typename Node::serializer_tag, typename Node::option, typename Node::profile, typename Node::path::template take_first_n<Node::path::size - 1>>;

struct payload_minimal_size_helper
{
    template<any_node Node>
    inline consteval static size_type payload_minimal_size();
};

template<any_node Node>
inline constexpr size_type payload_minimal_size = payload_minimal_size_helper::payload_minimal_size<Node>();

template<any_node Node>
inline constexpr size_type total_minimal_size = Node::prepend_minimal_size + payload_minimal_size<Node> + Node::postpend_minimal_size;

template<any_node Node>
consteval size_type payload_minimal_size_helper::payload_minimal_size()
{
    if constexpr (Node::primitive)
    {
        return Node::payload_minimal_size;
    }
    else
    {
        if constexpr (Node::size == 0)
            return 0;
        else
            return []<typename Ti, Ti... Is>(::std::integer_sequence<Ti, Is...>) {
                return (total_minimal_size<typename Node::template at<node_index<Is>>> + ...);
            }(::std::make_index_sequence<Node::size>{});
    }
}

#ifndef NA_SERIALIZER_HIDE_IMPL
struct prepend_minimal_offset_helper
{
    template<any_node Node>
    inline consteval static size_type offset_from_parent_begin();

    template<any_node Node>
    inline consteval static size_type prepend_minimal_offset()
    {
        if constexpr (Node::path::size == 0)
        {
            return 0;
        }
        else
        {
            using Parent = node_parent<Node>;
            return prepend_minimal_offset<Parent>() + offset_from_parent_begin<Node>();
        }
    }
};
#endif  // NA_SERIALIZER_HIDE_IMPL

template<any_node Node>
inline constexpr size_type prepend_minimal_offset =
#ifndef NA_SERIALIZER_HIDE_IMPL
    prepend_minimal_offset_helper::prepend_minimal_offset<Node>();
#else
    1;
#endif

template<any_node Node>
inline constexpr size_type payload_minimal_offset = prepend_minimal_offset<Node> + Node::prepend_minimal_size;

template<any_node Node>
inline constexpr size_type postpend_minimal_offset = payload_minimal_offset<Node> + payload_minimal_size<Node>;

template<any_node Node, node_index GetIndexInParent>
consteval auto flat_next_n_helper::next_node()
{
    if constexpr (GetIndexInParent < Node::size)
    {
        using Next = typename Node::template at<GetIndexInParent>;
        return ::std::declval<Next>();
    }
    else
    {
        return next_node<node_parent<Node>, path_last<typename Node::paths>::index + 1>();
    }
}

#ifndef NA_SERIALIZER_HIDE_IMPL
template<any_node Node>
consteval size_type prepend_minimal_offset_helper::offset_from_parent_begin()
{
    using Parent = node_parent<Node>;
    return []<typename Ti, Ti... Is>(::std::integer_sequence<Ti, Is...>) {
        if constexpr (sizeof...(Is) == 0)
        {
            return Parent::prepend_minimal_size;
        }
        else
        {
            return Parent::prepend_minimal_size + (total_minimal_size<typename Parent::template at<node_index<Is>>> + ...);
        }
    }(::std::make_index_sequence<path_last<typename Node::path>::index>{});
}
#endif

//
// node traits
//

template<typename T>
struct is_node : ::std::false_type
{};

template<any_serializer_tag S, any_serialize_option Option, any_serializer_profile Profile, any_node_path Path>
struct is_node<node<S, Option, Profile, Path>> : ::std::true_type
{};

//
// serialize sequence
//

struct no_context
{};

enum class serialize_steps
{
    prepend,
    payload,
    postpend,

    payload_directcopy,

    all
};

template<typename T>
struct is_serialize_step : ::std::false_type
{};

template<any_node Node, serialize_steps Step>
struct serialize_step
{
    using node = Node;
    constexpr static auto step = Step;

    template<serialize_steps S>
    using with_step = serialize_step<Node, S>;
};

template<any_node Node, serialize_steps Step>
struct is_serialize_step<serialize_step<Node, Step>> : ::std::true_type
{};

template<any_node... Nodes>
struct serialize_step_directcopy
{
    constexpr static size_type size = sizeof...(Nodes);
    constexpr static auto step = serialize_steps::payload_directcopy;
    template<size_type I>
    requires(I < size) using at = ::std::tuple_element_t<I, ::std::tuple<Nodes...>>;

    template<any_node... Ns>
    using append = serialize_step_directcopy<Nodes..., Ns...>;

    struct length_helper
    {
        inline consteval auto length()
        {
            if constexpr (size == 1)
            {
                return payload_minimal_size<at<0>>;
            }
            else
            {
                return (payload_minimal_size<Nodes> + ...);
            }
        }
    };

    constexpr static size_type length = length_helper::length();  // todo: test
};

template<any_node... Nodes>
struct is_serialize_step<serialize_step_directcopy<Nodes...>> : ::std::true_type
{};

template<typename T>
concept any_serialize_step = is_serialize_step<T>::value;

template<any_serialize_step... Steps>
struct serialize_step_list
{
    constexpr static size_type size = sizeof...(Steps);
    template<size_type I>
    requires(I < size) using at = ::std::tuple_element_t<I, ::std::tuple<Steps...>>;
};

struct generate_serialize_step_list_helper
{
    template<any_node Node, typename T, T... Is>
    inline consteval static auto generate_serialize_step_list_generate_composite_payload(::std::integer_sequence<T, Is...>);

    template<any_node Node>
    inline consteval static auto generate_serialize_step_list_generate()
    {
        auto prepend{
            []() {
                if constexpr (Node::prepend_minimal_size == 0 && Node::prepend_fixed_size)
                    return ::std::tuple<>{};
                else
                    return ::std::tuple<serialize_step<Node, serialize_steps::prepend>>{};
            }()};
        auto postpend{
            []() {
                if constexpr (Node::postpend_minimal_size == 0 && Node::postpend_fixed_size)
                    return ::std::tuple<>{};
                else
                    return ::std::tuple<serialize_step<Node, serialize_steps::postpend>>{};
            }()};

        if constexpr (Node::primitive)
        {
            auto payload{[]() {
                if constexpr (payload_minimal_size<Node> == 0 && Node::payload_fixed_size)
                    return ::std::tuple<>{};
                else if constexpr (Node::payload_memory_compatible)
                    return ::std::tuple<serialize_step_directcopy<Node>>{};
                else
                    return ::std::tuple<serialize_step<Node, serialize_steps::payload>>{};
            }()};
            return ::std::tuple_cat(prepend, payload, postpend);
        }
        else
        {
            return ::std::tuple_cat(prepend, generate_serialize_step_list_generate_composite_payload<Node>(::std::make_index_sequence<Node::size>{}), postpend);
        }
    }

    template<without_cvr Tuple, size_type Index>
    inline consteval static auto tuple_remove_at()
    {
        constexpr auto size{::std::tuple_size_v<Tuple>};
        static_assert(Index < size);
        return []<typename Ti, Ti... Is, Ti... Is2>(::std::integer_sequence<Ti, Is...>, ::std::integer_sequence<Ti, Is2...>) {
            using return_t = ::std::tuple<::std::tuple_element_t<Is, Tuple>..., ::std::tuple_element_t<Is2 + Index + 1, Tuple>...>;
            return return_t{};
        }(::std::make_index_sequence<Index>{}, ::std::make_index_sequence<size - Index - 1>{});
    }

    template<without_cvr Tuple, typename New, size_type Index>
    inline consteval static auto tuple_replace_at()
    {
        constexpr auto size{::std::tuple_size_v<Tuple>};
        static_assert(Index < size);
        return []<typename Ti, Ti... Is, Ti... Is2>(::std::integer_sequence<Ti, Is...>, ::std::integer_sequence<Ti, Is2...>) {
            using return_t = ::std::tuple<::std::tuple_element_t<Is, Tuple>..., New, ::std::tuple_element_t<Is2 + Index + 1, Tuple>...>;
            return return_t{};
        }(::std::make_index_sequence<Index>{}, ::std::make_index_sequence<size - Index - 1>{});
    }

    template<without_cvr Tuple>
    inline consteval static auto generate_serialize_step_list_compact()
    {
        if constexpr (::std::tuple_size_v<Tuple> <= 1)
            return Tuple{};
        else
        {
            auto compact_func = []<typename Tpl, size_type Index = 1>(auto self)
            {
                static_assert(Index <= ::std::tuple_size_v<Tuple>);
                if constexpr (Index == ::std::tuple_size_v<Tuple>)
                    return Tpl{};
                else
                {
                    using This = ::std::tuple_element_t<Index, Tpl>;
                    using Last = ::std::tuple_element_t<Index - 1, Tpl>;
                    if constexpr (Last::step == serialize_steps::payload_directcopy && This::step == serialize_steps::payload_directcopy)
                    {
                        static_assert(This::size == 1);
                        using NewLast = Last::template append<typename This::template at<0>>;
                        constexpr auto replaced{tuple_replace_at<Tpl, NewLast, Index - 1>()};
                        constexpr auto removed{tuple_remove_at<Tpl, Index>()};
                        return self.template operator()<Tpl, Index>(self);
                    }
                    else
                    {
                        return self.template operator()<Tpl, Index + 1>(self);
                    }
                }
            };
            return compact_func.template operator()<Tuple>(compact_func);
        }
    }

    template<without_cvr Tuple>
    inline consteval static auto generate_serialize_step_list_expand()
    {
        if constexpr (::std::tuple_size_v<Tuple> < 1)
        {
            return Tuple{};
        }
        else if constexpr (::std::tuple_size_v<Tuple> == 1)
        {
            using This = ::std::tuple_element_t<0, Tuple>;
            if constexpr (This::step == serialize_steps::payload)
            {
                return ::std::tuple<typename This::template with_step<serialize_steps::all>>{};
            }
            else
            {
                return Tuple{};
            }
        }
        else
        {
            auto expand_payload = []<typename Tpl, size_type Index = 0>(auto self)
            {
                static_assert(Index <= ::std::tuple_size_v<Tpl>);
                if constexpr (Index == ::std::tuple_size_v<Tpl>)
                {
                    return Tpl{};
                }
                else
                {
                    static_assert(::std::tuple_size_v<Tpl> > 1);
                    using This = ::std::tuple_element_t<Index, Tpl>;
                    if constexpr (This::step != serialize_steps::payload)
                    {
                        return self.template operator()<Tpl, Index + 1>(self);
                    }
                    else
                    {
                        auto remove_postpend_tuple_func{
                            []() {
                                if constexpr (Index == ::std::tuple_size_v<Tpl> - 1)
                                {
                                    return Tpl{};
                                }
                                else
                                {
                                    using Next = ::std::tuple_element_t<Index + 1, Tpl>;
                                    if constexpr (::std::same_as<typename Next::node, typename This::node> && Next::step == serialize_steps::postpend)
                                        return tuple_remove_at<Tpl, Index + 1>();
                                    else
                                        return Tpl{};
                                }
                            }};
                        using RemovePostpendTuple = decltype(remove_postpend_tuple_func());
                        using WS = This::template with_step<serialize_steps::all>;

                        using ReplacedTuple = decltype(tuple_replace_at<RemovePostpendTuple, WS, Index>());

                        constexpr auto do_remove_prepend{
                            []() {
                                if constexpr (Index == 0)
                                {
                                    return false;
                                }
                                else
                                {
                                    using Last = ::std::tuple_element_t<Index - 1, ReplacedTuple>;
                                    if constexpr (::std::same_as<typename Last::node, typename This::node> && Last::step == serialize_steps::prepend)
                                        return true;
                                    else
                                        return false;
                                }
                            }()};

                        if constexpr (do_remove_prepend)
                            return self.template operator()<decltype(tuple_remove_at<ReplacedTuple, Index - 1>()), Index>(self);
                        else
                            return self.template operator()<ReplacedTuple, Index + 1>(self);
                    }
                }
            };
            return expand_payload.template operator()<Tuple>(expand_payload);
        }
    }

    template<any_node Node>
    inline static auto generate_serialize_step_list()
    {
        using Generate = decltype(generate_serialize_step_list_generate<Node>());

        using Compact = decltype(generate_serialize_step_list_compact<Generate>());

        using Expand = decltype(generate_serialize_step_list_expand<Compact>());

        return []<typename... Ts>(::std::tuple<Ts...> x) {
            return serialize_step_list<Ts...>{};
        }(Expand{});
    }
};

template<any_node Node, typename T, T... Is>
consteval auto generate_serialize_step_list_helper::generate_serialize_step_list_generate_composite_payload(::std::integer_sequence<T, Is...>)
{
    static_assert(Node::composite);
    return ::std::tuple_cat(generate_serialize_step_list_generate<typename Node::template at<node_index<Is>>>()...);
}

template<any_node Node>
using generate_serialize_step_list = decltype(generate_serialize_step_list_helper::generate_serialize_step_list<Node>());

struct generate_context_tuple_helper
{
    template<typename This, size_type I>
    inline static auto generate_context_tuple_one()
    {
        if constexpr (This::step == serialize_steps::prepend)
        {
            using Context = This::node::context;
            if constexpr (!::std::same_as<Context, no_context>)
                return ::std::tuple<Context>{};
            else
                return ::std::tuple<>{};
        }
        else
        {
            return ::std::tuple<>{};
        }
    }
    template<typename Steps, typename Ti, Ti... Is>
    inline consteval static auto generate_context_tuple(::std::integer_sequence<Ti, Is...>)
    {
        return ::std::tuple_cat(generate_context_tuple_one<typename Steps::template at<Is>, Is>()...);
    }

    template<typename This>
    inline consteval static size_type step_context_index_impl()
    {
        if constexpr (This::step == serialize_steps::prepend)
        {
            using Context = This::node::context;
            if constexpr (!::std::same_as<Context, no_context>)
                return 1;
            else
                return 0;
        }
        else
        {
            return 0;
        }
    }
    template<typename Steps, typename Ti, Ti... Is>
    inline consteval static size_type step_context_index(::std::integer_sequence<Ti, Is...>)
    {
        if constexpr (sizeof...(Is) == 1)
            return step_context_index_impl<typename Steps::template at<0>>();
        else
            return (step_context_index_impl<typename Steps::template at<Is>>() + ...);
    }
    template<typename Steps, size_type Index>
    inline consteval static size_type step_context_index_wrapper()
    {
        if constexpr (Index == 0)
        {
            return 0;
        }
        else
        {
            return step_context_index<Steps>(::std::make_index_sequence<Index - 1>{});
        }
    }
};

template<typename Steps>
using generate_context_tuple = decltype(generate_context_tuple_helper::generate_context_tuple<Steps>(::std::make_index_sequence<Steps::size>{}));

template<typename Steps, size_type Index>
requires(Index < Steps::size) inline constexpr static size_type step_context_index = generate_context_tuple_helper::step_context_index_wrapper<Steps, Index>();

template<typename Steps, size_type Index>
inline constexpr bool deserialize_one(auto& value, ::std::byte const* start, ::std::size_t reversed, auto& error_code, size_type& offset, auto& contexts)
{
    using This = Steps::template at<Index>;
    static_assert(This::step != serialize_steps::payload);
    if constexpr (This::step == serialize_steps::prepend)
    {
        if constexpr (::std::same_as<typename This::node::context, no_context>)
        {
            no_context dummy{};
            return This::node::deserialize_prepend(value, start, offset, reversed, dummy, error_code);
        }
        else
        {
            return This::node::deserialize_prepend(value, start, offset, reversed, ::std::get<step_context_index<Steps, Index>>(contexts), error_code);
        }
    }
    else if constexpr (This::step == serialize_steps::postpend)
    {
        if constexpr (::std::same_as<typename This::node::context, no_context>)
        {
            no_context dummy{};
            return This::node::deserialize_postpend(value, start, offset, reversed, dummy, error_code);
        }
        else
        {
            return This::node::deserialize_postpend(value, start, offset, reversed, ::std::get<step_context_index<Steps, Index>>(contexts), error_code);
        }
    }
    else if constexpr (This::step == serialize_steps::all)
    {
        return This::node::deserialize_all(value, start, offset, reversed, error_code);
    }
    else
    {
        static_assert(This::step == serialize_steps::payload_directcopy);
        ::std::memcpy(start + payload_minimal_offset<typename This::node> + offset, ::std::addressof(This::node::payload_reference(value)), This::length);
        return true;
    }
}
template<typename Steps, typename Ti, Ti... Is>
inline constexpr bool deserialize_impl(auto& value, ::std::byte const* start, ::std::size_t reversed, auto& error_code, ::std::integer_sequence<Ti, Is...>)
{
    generate_context_tuple<Steps> contexts{};
    ::std::size_t offset{0};
    return (deserialize_one<Steps, Is>(value, start, reversed, error_code, offset, contexts) && ...);
}
template<typename S, typename Option, ::std::size_t E>
inline constexpr bool deserialize(auto& value, ::std::span<::std::byte const, E> view, auto& error_code) noexcept
{
    using NodeN = node<S, Option, serializer_profile<operations::serialize>, node_path<::std::remove_reference_t<decltype(value)>>>;
    auto const source{view.data()};
    ::std::size_t source_length{0};
    if constexpr (E == ::std::dynamic_extent)
    {
        source_length = view.size();
    }
    else
    {
        source_length = E;
    }
    using SS = ::std::make_signed_t<::std::size_t>;
    SS reversed{static_cast<SS>(source_length) - static_cast<SS>(total_minimal_size<NodeN>)};
    if (reversed < 0) [[unlikely]]
    {
        return false;
    }
    using List = generate_serialize_step_list<NodeN>;
    auto result{deserialize_impl<List>(value, source, reversed, error_code, ::std::make_index_sequence<List::size>{})};
    if (!result) [[unlikely]]
    {
        return false;
    }
    else
    {
        return true;
    }
}

}  // namespace na::serializer