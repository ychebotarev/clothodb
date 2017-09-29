#pragma once

namespace cdb{

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

    constexpr const V& as_value() const
    {
        return value;
    }

    constexpr const E& as_error() const
    {
        return err;
    }

    static constexpr auto from_value(V const& v) 
    {
        return Result<V, E>{ cdb::make_value(v) };
    }
    static constexpr auto from_value(V&& v)
    {
        return Result<V, E>{ cdb::make_value(std::move(v)) };
    }

    static constexpr auto from_error(E const& e)
    {
        return Result<V, E>{ cdb::make_error(e) };
    }

    static constexpr auto from_error(E&& e)
    {
        return Result<V, E>{ cdb::make_error(std::move(e)) };
    }

    constexpr bool is_value()const { return m_has_value; }
    constexpr bool is_error()const { return !m_has_value; }
};

}