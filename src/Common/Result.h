#pragma once

namespace clothodb {
namespace common {

template <class T> struct Maybe;

template <> struct Maybe<void> {};

template <class T> struct Maybe 
{
    typedef T inner_Type;

    union
    {
        T value;
    };

    bool const m_has_value = 0;

    constexpr Maybe() : m_has_value{ 0 } {}

    constexpr Maybe(T const& value) : value{ value }, m_has_value{ 1 } {}
    constexpr Maybe(T&& value) : value{ std::move(value) }, m_has_value{ 1 } {}

    Maybe(Maybe<void>) : m_has_value{ 0 } {}

    Maybe(Maybe<T> const &o) : m_has_value{ o.has_value }
    {
        if (o.m_has_value) new (&value)T(o.value);
    }

    ~Maybe() 
    {
        if (m_has_value) value.~T();
    }

    constexpr T get(T defaultValue) const
    {
        return (m_has_value)? value : defaultValue;
    }

    constexpr operator bool()const { return m_has_value; }
        
    constexpr bool has_value()const { return m_has_value; }
};

template <class T> auto maybe(T value) -> Maybe<T> { return{ value }; }
template <class T = void> auto maybe() -> Maybe<T> { return{}; }

template<class V>
struct Value 
{
    V value;
};

template<class V>
constexpr Value<V> make_value(V const& value) 
{
    return{ value };
}

template<class V>
Value<V> make_value(V&& value)
{
    return{ std::move(value) };
}

template<class E>
struct Error 
{
    E value;
};

template<class E>
constexpr Error<E> make_error(E const& e) 
{
    return{ e };
}


template<class V, class E>
struct Result 
{
    typedef V inner_Type;

    union
    {
        V value;
        E err;
    };

    bool const m_has_value = 0;

    constexpr Result(Value<V> const& v)
        : value{ v.value }
        , m_has_value(1)
    {}
        
    Result(Value<V>&& v)
        : value{ std::move(v.value) }
        , m_has_value(1)
    {}

    constexpr Result(Error<E> const& e)
        : err{ e.value }
        , m_has_value(0)
    {}

    Result(Error<E>&& e)
        : err{ std::move(e.value) }
        , m_has_value(0)
    {}

    Result(Result<V, E> const& result) : m_has_value(result.m_has_value)
    {
        if (m_has_value)
            new (&value)V(result.value);
        else
            new (&err)E(result.err);
    }

    Result(Result<V, E>&& result) : m_has_value(result.m_has_value)
    {
        if (m_has_value)
            new (&value)V(std::move(result.value));
        else
            new (&err)E(std::move(result.err));
    }

    ~Result() 
    {
        if (m_has_value)
            value.~V();
        else
            err.~E();
    }

    constexpr auto result() const -> Maybe<V> 
    {
        return (m_has_value) ? maybe(value) : maybe();
    }

    constexpr auto error() const -> Maybe<E> 
    {
        return (!m_has_value) ? maybe(err) : maybe();
    }

    static constexpr auto from_value(V const& v) 
    {
        return Result<V, E>{ clothodb::common::make_value(v) };
    }
    static constexpr auto from_value(V&& v)
    {
        return Result<V, E>{ clothodb::common::make_value(std::move(v)) };
    }

    static constexpr auto from_error(E const& e)
    {
        return Result<V, E>{ clothodb::common::make_error(e) };
    }

    static constexpr auto from_error(E&& e)
    {
        return Result<V, E>{ clothodb::common::make_error(std::move(e)) };
    }

    constexpr bool is_error()const { return !m_has_value; }
};

}}
