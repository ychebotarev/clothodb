#pragma once

namespace incolun {
namespace clothodb {

    template <class T> struct Maybe;

    template <> struct Maybe<void> {};

    template <class T> struct Maybe 
    {
        typedef T inner_Type;

        union
        {
            T value;
        };

        bool const hasValue = 0;

        constexpr Maybe() : hasValue{ 0 } {}

        constexpr Maybe(T const& value) : value{ value }, hasValue{ 1 } {}
        constexpr Maybe(T&& value) : value{ std::move(value) }, hasValue{ 1 } {}

        Maybe(Maybe<void>) : hasValue{ 0 } {}

        Maybe(Maybe<T> const &o) : hasValue{ o.hasValue } 
        {
            if (o.hasValue) new (&value)T(o.value);
        }

        ~Maybe() 
        {
            if (hasValue) value.~T();
        }

        constexpr T get(T defaultValue) const
        {
            return (hasValue)? value : defaultValue;
        }

        constexpr operator bool()const { return hasValue; }
        
        constexpr bool HasValue()const { return hasValue; }
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
        typedef T inner_Type;

        union
        {
            V value;
            E err;
        };

        bool const hasValue = 0;

        constexpr Result(Value<V> const& v)
            : value{ v.value }
            , hasValue(1)
        {}
        
        Result(Value<V>&& v)
            : value{ std::move(v.value) }
            , hasValue(1)
        {}

        constexpr Result(Error<E> const& e)
            : err{ e.value }
            , hasValue(0)
        {}

        Result(Error<E>&& e)
            : err{ std::move(e.value) }
            , hasValue(0)
        {}

        Result(Result<V, E> const& result) : hasValue(result.hasValue) 
        {
            if (hasValue)
                new (&value)V(result.value);
            else
                new (&err)E(result.err);
        }

        Result(Result<V, E>&& result) : hasValue(result.hasValue) 
        {
            if (hasValue)
                new (&value)V(std::move(result.value));
            else
                new (&err)E(std::move(result.err));
        }

        ~Result() 
        {
            if (hasValue)
                value.~V();
            else
                err.~E();
        }

        constexpr auto result() const -> Maybe<V> 
        {
            return (hasValue) ? maybe(value) : maybe();
        }

        constexpr auto error() const -> Maybe<E> 
        {
            return (!hasValue) ? maybe(err) : maybe();
        }

        static constexpr auto fromValue(V const& v) 
        {
            return Result<V, E>{ incolun::clothodb::make_value(v) };
        }
        static constexpr auto fromValue(V&& v)
        {
            return Result<V, E>{ incolun::clothodb::make_value(std::move(v)) };
        }

        static constexpr auto fromError(E const& e)
        {
            return Result<V, E>{ incolun::clothodb::make_error(e) };
        }

        static constexpr auto fromError(E&& e)
        {
            return Result<V, E>{ incolun::clothodb::make_error(std::move(e)) };
        }

        constexpr bool IsError()const { return !hasValue; }
    };
}}
