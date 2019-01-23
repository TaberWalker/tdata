#ifndef __TDATA_HPP__
#define __TDATA_HPP__

#include "variant.hpp"

#include <cstdint>
#include <cmath>
#include <cassert>
#include <limits>
#include <string>
#include <vector>
#include <type_traits>


namespace tdata {
    enum class Type : char
    {
        kUnknown    = 0,
        kInt        = 'i',
        kReal       = 'r',
        kStr        = 's',
        kVInt       = 'I',
        kVReal      = 'R',
        kVStr       = 'S',
    };

    using int_t = int64_t;
    using real_t = double;
    using str_t = std::string;
    using vint_t = std::vector<int_t>;
    using vreal_t = std::vector<real_t>;
    using vstr_t = std::vector<str_t>;
    using variant_t = mapbox::util::variant<int_t, real_t, str_t, vint_t, vreal_t, vstr_t>;

    static const str_t::value_type kBegSepChar = '^';
    static const str_t::value_type kFieldSepChar = ':';
    static const str_t::value_type kEndSepChar = '$';
    static const str_t::value_type kTransChar = '\\';

    static const str_t kBegSepStr = "^";
    static const str_t kFieldSepStr = ":";
    static const str_t kEndSepStr = "$";

    namespace detail {
        struct equal_comp
        {
            template <typename T>
            bool operator()(T const& lhs, T const& rhs) const
            {
                return lhs == rhs;
            }

            bool operator()(const real_t& lhs, const real_t& rhs) const
            {
                return fabs(lhs - rhs) < std::numeric_limits<real_t>::epsilon();
            }
            bool operator()(const vreal_t& lhs, const vreal_t& rhs) const
            {
                if (lhs.size() != rhs.size())
                {
                    return false;
                }
                return std::equal(lhs.begin(), lhs.end(), rhs.begin(), *this);
            }
            bool operator()(const vint_t& lhs, const vint_t& rhs) const
            {
                if (lhs.size() != rhs.size())
                {
                    return false;
                }
                return std::equal(lhs.begin(), lhs.end(), rhs.begin());
            }
            bool operator()(const vstr_t& lhs, const vstr_t& rhs) const
            {
                if (lhs.size() != rhs.size())
                {
                    return false;
                }
                return std::equal(lhs.begin(), lhs.end(), rhs.begin());
            }
        };

        struct StrCoder
        {
            static str_t Encode(const str_t& s)
            {
                str_t r;
                r.reserve(s.size());
                for (const auto c : s)
                {
                    if (c == kFieldSepChar || c == kEndSepChar)
                    {
                        r.push_back(kTransChar);
                    }
                    r.push_back(c);
                }
                return std::move(r);
            }
            
            static str_t Decode(const str_t::value_type* b, const str_t::value_type* e)
            {
                str_t r;
                r.reserve(e - b);
                for (; b < e; ++b)
                {
                    const auto cur = *b;
                    const auto nex = *(b + 1);
                    if (cur == kTransChar && (nex == kFieldSepChar || nex == kEndSepChar))
                    {
                        continue;
                    }
                    r.push_back(cur);
                }
                return std::move(r);
            }

            static Type GetType(const str_t& s, str_t::size_type beg)
            {
                const auto size = s.size();
                return beg + 1 >= size ? Type::kUnknown : static_cast<Type>(s[++beg]);
            }

            static bool CheckTypeAndFindEnd(const str_t& s, Type type, str_t::size_type& beg, str_t::size_type& end)
            {
                const auto size = s.size();
                if (beg + 1 >= size || s[beg] != kBegSepChar || static_cast<Type>(s[++beg]) != type)
                {
                    return false;
                }
                end = ++beg;
                while (++end < size && (s[end] != kEndSepChar || s[end - 1] == kTransChar))
                {
                }
                return end != size;
            }
        };
    }

    template <typename T, typename = void>
    struct tdata_traits : std::false_type {};

    template <typename T>
    struct tdata_traits<T, typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value>::type> : std::true_type
    {
        using value_type = int_t;
        using return_type = int_t;
        static constexpr auto enum_value = Type::kInt;
        static const value_type null_value;

        static void ToStr(return_type v, str_t& s) { s += kBegSepStr + str_t(1, static_cast<str_t::value_type>(enum_value)) + std::to_string(v) + kEndSepStr; }
        static bool FromStr(value_type& v, const str_t& s, str_t::size_type* p = nullptr)
        {
            str_t::size_type beg = (nullptr != p ? *p : 0);
            auto end = beg;
            if (!detail::StrCoder::CheckTypeAndFindEnd(s, enum_value, beg, end))
            {
                return false;
            }
            v = std::strtoll(&s[beg], nullptr, 10);
            if (nullptr != p)
            {
                *p = ++end;
            }
            return true;
        }
    };
    template <typename T>
    const int_t tdata_traits<T, typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value>::type>::null_value = value_type();

    template <typename T>
    struct tdata_traits<T, typename std::enable_if<std::is_floating_point<typename std::decay<T>::type>::value>::type> : std::true_type
    {
        using value_type = real_t;
        using return_type = real_t;
        static constexpr auto enum_value = Type::kReal;
        static const value_type null_value;

        static void ToStr(return_type v, str_t& s) { s += kBegSepStr + str_t(1, static_cast<str_t::value_type>(enum_value)) + std::to_string(v) + kEndSepStr; }
        static bool FromStr(value_type& v, const str_t& s, str_t::size_type* p = nullptr)
        {
            str_t::size_type beg = (nullptr != p ? *p : 0);
            auto end = beg;
            if (!detail::StrCoder::CheckTypeAndFindEnd(s, enum_value, beg, end))
            {
                return false;
            }
            v = std::strtod(&s[beg], nullptr);
            if (nullptr != p)
            {
                *p = ++end;
            }
            return true;
        }
    };
    template <typename T>
    const real_t tdata_traits<T, typename std::enable_if<std::is_floating_point<typename std::decay<T>::type>::value>::type>::null_value = value_type();

    template <typename T>
    struct tdata_traits<T, typename std::enable_if<
        std::is_same<typename std::remove_cv<typename std::decay<T>::type>::type, str_t>::value
        || std::is_same<typename std::remove_cv<typename std::decay<T>::type>::type, char*>::value
        || std::is_same<typename std::remove_cv<typename std::decay<T>::type>::type, const char*>::value
    >::type> : std::true_type
    {
        using value_type = str_t;
        using return_type = const str_t&;
        static constexpr auto enum_value = Type::kStr;
        static const value_type null_value;

        static void ToStr(return_type v, str_t& s) { s += kBegSepStr + str_t(1, static_cast<str_t::value_type>(enum_value)) + detail::StrCoder::Encode(v) + kEndSepStr; }
        static bool FromStr(value_type& v, const str_t& s, str_t::size_type* p = nullptr)
        {
            str_t::size_type beg = (nullptr != p ? *p : 0);
            auto end = beg;
            if (!detail::StrCoder::CheckTypeAndFindEnd(s, enum_value, beg, end))
            {
                return false;
            }
            v = detail::StrCoder::Decode(&s[beg], &s[end]);
            if (nullptr != p)
            {
                *p = ++end;
            }
            return true;
        }
    };
    template <typename T>
    const str_t tdata_traits<T, typename std::enable_if<
        std::is_same<typename std::remove_cv<typename std::decay<T>::type>::type, str_t>::value
        || std::is_same<typename std::remove_cv<typename std::decay<T>::type>::type, char*>::value
        || std::is_same<typename std::remove_cv<typename std::decay<T>::type>::type, const char*>::value
    >::type>::null_value = value_type();

    template <typename T>
    struct tdata_traits<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, vint_t>::value>::type> : std::true_type
    {
        using value_type = vint_t;
        using return_type = const vint_t&;
        static constexpr auto enum_value = Type::kVInt;
        static const value_type null_value;

        static void ToStr(return_type v, str_t& s)
        {
            s += kBegSepStr + str_t(1, static_cast<str_t::value_type>(enum_value)) + std::to_string(v.size());
            for (const auto n : v)
            {
                s += kFieldSepStr + std::to_string(n);
            }
            s += kEndSepStr;
        }
        static bool FromStr(value_type& v, const str_t& s, str_t::size_type* p = nullptr)
        {
            str_t::size_type beg = (nullptr != p ? *p : 0);
            auto end = beg;
            if (!detail::StrCoder::CheckTypeAndFindEnd(s, enum_value, beg, end))
            {
                return false;
            }
            str_t::value_type* begptr = nullptr;
            const auto endptr = &s[end];
            auto size = std::strtoll(&s[beg], &begptr, 10);
            v.reserve(v.size() + static_cast<typename value_type::size_type>(size));
            while (0 != size--)
            {
                assert(begptr < endptr);
                v.push_back(std::strtoll(++begptr, &begptr, 10));
            }
            if (nullptr != p)
            {
                *p = ++end;
            }
            return true;
        }
    };
    template <typename T>
    const vint_t tdata_traits<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, vint_t>::value>::type>::null_value = value_type();

    template <typename T>
    struct tdata_traits<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, vreal_t>::value>::type> : std::true_type
    {
        using value_type = vreal_t;
        using return_type = const vreal_t&;
        static constexpr auto enum_value = Type::kVReal;
        static const value_type null_value;

        static void ToStr(return_type v, str_t& s)
        {
            s += kBegSepStr + str_t(1, static_cast<str_t::value_type>(enum_value)) + std::to_string(v.size());
            for (const auto n : v)
            {
                s += kFieldSepStr + std::to_string(n);
            }
            s += kEndSepStr;
        }
        static bool FromStr(value_type& v, const str_t& s, str_t::size_type* p = nullptr)
        {
            str_t::size_type beg = (nullptr != p ? *p : 0);
            auto end = beg;
            if (!detail::StrCoder::CheckTypeAndFindEnd(s, enum_value, beg, end))
            {
                return false;
            }
            str_t::value_type* begptr = nullptr;
            const auto endptr = &s[end];
            auto size = std::strtoll(&s[beg], &begptr, 10);
            v.reserve(v.size() + static_cast<typename value_type::size_type>(size));
            while (0 != size--)
            {
                assert(begptr < endptr);
                v.push_back(std::strtod(++begptr, &begptr));
            }
            if (nullptr != p)
            {
                *p = ++end;
            }
            return true;
        }
    };
    template <typename T>
    const vreal_t tdata_traits<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, vreal_t>::value>::type>::null_value = value_type();

    template <typename T>
    struct tdata_traits<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, vstr_t>::value>::type> : std::true_type
    {
        using value_type = vstr_t;
        using return_type = const vstr_t&;
        static constexpr auto enum_value = Type::kVStr;
        static const value_type null_value;

        static void ToStr(return_type v, str_t& s)
        {
            s += kBegSepStr + str_t(1, static_cast<str_t::value_type>(enum_value)) + std::to_string(v.size());
            for (const auto& n : v)
            {
                s += kFieldSepStr + detail::StrCoder::Encode(n);
            }
            s += kEndSepStr;
        }
        static bool FromStr(value_type& v, const str_t& s, str_t::size_type* p = nullptr)
        {
            str_t::size_type beg = (nullptr != p ? *p : 0);
            auto end = beg;
            if (!detail::StrCoder::CheckTypeAndFindEnd(s, enum_value, beg, end))
            {
                return false;
            }
            str_t::value_type* begptr = nullptr;
            const auto endptr = &s[end];
            auto size = std::strtoll(&s[beg], &begptr, 10);
            v.reserve(v.size() + static_cast<typename value_type::size_type>(size));
            while (0 != size--)
            {
                assert(begptr < endptr);
                auto ptr = ++begptr;
                while (++ptr < endptr && (*ptr != kFieldSepChar || *(ptr - 1) == kTransChar))
                {
                }
                v.push_back(detail::StrCoder::Decode(begptr, ptr));
                begptr = ptr;
            }
            if (nullptr != p)
            {
                *p = ++end;
            }
            return true;
        }
    };
    template <typename T>
    const vstr_t tdata_traits<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, vstr_t>::value>::type>::null_value = value_type();

    class TData
    {
    public:
        TData() = default;
        TData(const TData&) = default;
        TData(TData&&) = default;
        TData& operator= (const TData&) = default;
        TData& operator= (TData&&) = default;

        template <typename T, typename = typename std::enable_if<tdata_traits<T>::value>::type>
        explicit TData(T&& v) : type_(tdata_traits<T>::enum_value), data_(typename tdata_traits<T>::value_type(v)) {}

        void Clear()
        {
            switch (GetType())
            {
            case Type::kInt: SetValue(tdata_traits<int_t>::null_value); break;
            case Type::kReal: SetValue(tdata_traits<real_t>::null_value); break;
            case Type::kStr: SetValue(tdata_traits<str_t>::null_value); break;
            case Type::kVInt: SetValue(tdata_traits<vint_t>::null_value); break;
            case Type::kVReal: SetValue(tdata_traits<vreal_t>::null_value); break;
            case Type::kVStr: SetValue(tdata_traits<vstr_t>::null_value); break;
            default: break;
            }
        }

        bool IsNull() const
        {
            switch (GetType())
            {
            case Type::kInt: return tdata_traits<int_t>::null_value == GetValue<int_t>();
            case Type::kReal: return tdata_traits<real_t>::null_value == GetValue<real_t>();
            case Type::kStr: return tdata_traits<str_t>::null_value == GetValue<str_t>();
            case Type::kVInt: return tdata_traits<vint_t>::null_value == GetValue<vint_t>();
            case Type::kVReal: return tdata_traits<vreal_t>::null_value == GetValue<vreal_t>();
            case Type::kVStr: return tdata_traits<vstr_t>::null_value == GetValue<vstr_t>();
            default: break;
            }
            return true;
        }

        str_t ToStr() const
        {
            str_t str;
            switch (GetType())
            {
            case Type::kInt: tdata_traits<int_t>::ToStr(GetValue<int_t>(), str); break;
            case Type::kReal: tdata_traits<real_t>::ToStr(GetValue<real_t>(), str); break;
            case Type::kStr: tdata_traits<str_t>::ToStr(GetValue<str_t>(), str); break;
            case Type::kVInt: tdata_traits<vint_t>::ToStr(GetValue<vint_t>(), str); break;
            case Type::kVReal: tdata_traits<vreal_t>::ToStr(GetValue<vreal_t>(), str); break;
            case Type::kVStr: tdata_traits<vstr_t>::ToStr(GetValue<vstr_t>(), str); break;
            default: break;
            }
            return str;
        }

        static bool FromStr(TData& v, const str_t& s, str_t::size_type* p = nullptr)
        {
            switch (detail::StrCoder::GetType(s, nullptr != p ? *p : 0))
            {
            case Type::kInt: { int_t i; return tdata_traits<int_t>::FromStr(i, s, p) && v.SetValue(i); }
            case Type::kReal: { real_t r; return tdata_traits<real_t>::FromStr(r, s, p) && v.SetValue(r); }
            case Type::kStr: { str_t ss; return tdata_traits<str_t>::FromStr(ss, s, p) && v.SetValue(ss); }
            case Type::kVInt: { vint_t vi; return tdata_traits<vint_t>::FromStr(vi, s, p) && v.SetValue(vi); }
            case Type::kVReal: { vreal_t vr; return tdata_traits<vreal_t>::FromStr(vr, s, p) && v.SetValue(vr); }
            case Type::kVStr: { vstr_t vs; return tdata_traits<vstr_t>::FromStr(vs, s, p) && v.SetValue(vs); }
            default: return false;
            }
        }

        template <typename T>
        bool SetValue(T&& v)
        {
            if (GetType() == Type::kUnknown)
            {
                SetType(tdata_traits<T>::enum_value);
            }
            if (GetType() == tdata_traits<T>::enum_value)
            {
                data_.set<typename tdata_traits<T>::value_type>(v);
                return true;
            }
            return false;
        }

        template <typename T>
        typename tdata_traits<T>::return_type GetValue() const
        {
            if (GetType() == tdata_traits<T>::enum_value)
            {
                return data_.get<typename tdata_traits<T>::value_type>();
            }
            return tdata_traits<T>::null_value;
        }

        Type GetType() const { return type_; }
        const variant_t& GetData() const { return data_; }

    private:
        void SetType(Type type) { type_ = type; }

    private:
        Type type_ = Type::kUnknown;
        variant_t data_;
    };

    bool operator== (const TData& lhs, const TData& rhs)
    {
        if (lhs.GetType() != rhs.GetType())
        {
            return false;
        }
        assert(lhs.GetData().valid() && rhs.GetData().valid());
        if (lhs.GetData().which() != rhs.GetData().which())
        {
            return false;
        }
        mapbox::util::detail::comparer<variant_t, detail::equal_comp> visitor(lhs.GetData());
        return variant_t::visit(rhs.GetData(), visitor);
    }
    bool operator!= (const TData& lhs, const TData& rhs) { return !(lhs == rhs); }
}

#endif // !__TDATA_HPP__

