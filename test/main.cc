#include <iostream>
#include <iterator>
#include <iostream>
#include "../include/tdata.hpp"


#define K_JOIN(a, b) K_JOIN_HELPER(a, b)
#define K_JOIN_HELPER(a, b) a ## b

#define TEST_TYPE_DEF(type, name, value)                                                        \
    type name = type();                                                                         \
    tdata::TData K_JOIN(d_, name)(name);                                                        \
    K_JOIN(d_, name).SetValue(type(value));                                                     \
    std::cout << "null_value: " << tdata::tdata_traits<type>::null_value << std::endl;          \
    std::cout << "get value:" << K_JOIN(d_, name).GetValue<type>() << std::endl;                \
    std::cout << "to string: " << K_JOIN(d_, name).ToStr() << std::endl;                        \
    K_JOIN(d_, name).Clear();                                                                   \
    std::cout << std::boolalpha << "is null: " << K_JOIN(d_, name).IsNull() << std::endl;       \
    K_JOIN(d_, name).SetValue(type(value));                                                     \
                                                                                                \
    const type K_JOIN(c_, name) = type();                                                       \
    tdata::TData K_JOIN(cd_, name)(K_JOIN(c_, name));                                           \
    K_JOIN(cd_, name).SetValue(type(value));                                                    \
    std::cout << "null_value: " << tdata::tdata_traits<const type>::null_value << std::endl;    \
    std::cout << "get value:" << K_JOIN(cd_, name).GetValue<const type>() << std::endl;         \
    std::cout << "to string: " << K_JOIN(cd_, name).ToStr() << std::endl;                       \
    K_JOIN(cd_, name).Clear();                                                                  \
    std::cout << std::boolalpha << "is null: " << K_JOIN(cd_, name).IsNull() << std::endl;      \
    K_JOIN(cd_, name).SetValue(type(value));                                                    \
                                                                                                \
    std::cout << std::boolalpha << (K_JOIN(d_, name) == K_JOIN(cd_, name)) << std::endl;        \
    std::cout << "---------------------------------------------------------------" << std::endl;\
    do                                                                                          \
    {                                                                                           \
        tdata::TData t_a(type(value));                                                          \
        const tdata::TData t_b(t_a);                                                            \
        tdata::TData t_t_a(t_a);                                                                \
        const tdata::TData t_t_b(t_b);                                                          \
        tdata::TData t_tm_a(std::move(t_a));                                                    \
        tdata::TData t_tm_b(std::move(t_b));                                                    \
        const tdata::TData ct_tm_a(std::move(t_t_a));                                           \
        const tdata::TData ct_tm_b(std::move(t_t_b));                                           \
    } while (false)

int main()
{
    TEST_TYPE_DEF(int8_t, i8, 1);
    TEST_TYPE_DEF(int16_t, i16, 2);
    TEST_TYPE_DEF(int32_t, i32, 3);
    TEST_TYPE_DEF(int64_t, i64, 4);

    TEST_TYPE_DEF(uint8_t, u8, 1);
    TEST_TYPE_DEF(uint16_t, u16, 2);
    TEST_TYPE_DEF(uint32_t, u32, 3);
    TEST_TYPE_DEF(uint64_t, u64, 4);
    
    TEST_TYPE_DEF(float_t, f, 11);
    TEST_TYPE_DEF(double_t, d, 12);

    TEST_TYPE_DEF(std::string, str, "^:$");

    std::cout << typeid(tdata::tdata_traits<tdata::vint_t>::null_value).name() << std::endl;
    std::cout << typeid(tdata::tdata_traits<tdata::vreal_t>::null_value).name() << std::endl;
    std::cout << typeid(tdata::tdata_traits<tdata::vstr_t>::null_value).name() << std::endl;
    
    char c[] = "K";
    char* pc =  c;
    tdata::TData d_pc(pc);
    const char* cpc = pc;
    tdata::TData d_cpc(cpc);
    char* const pcc = pc;
    tdata::TData d_pcc(pcc);

    tdata::vint_t vi1 = {}, vi2 = { 1, 2, 3, 4, 5, 6 };
    tdata::TData vd_vi1(vi1), vd_vi2(vi2);
    std::cout << std::boolalpha << (vd_vi1 == vd_vi2) << std::endl;
    std::cout << vd_vi1.ToStr() << std::endl;
    std::cout << vd_vi2.ToStr() << std::endl;

    tdata::vreal_t vr1 = {}, vr2 = { 1.1, 2.2, 3.3, 4.4, 5.5, 6.6 };
    tdata::TData vd_vr1(vr1), vd_vr2(vr2);
    std::cout << std::boolalpha << (vd_vr1 == vd_vr2) << std::endl;
    std::cout << vd_vr1.ToStr() << std::endl;
    std::cout << vd_vr2.ToStr() << std::endl;

    tdata::vstr_t vs1 = {}, vs2 = { "^1:2$", "^3:4$", "^5:6$" };
    tdata::TData vd_vs1(vs1), vd_vs2(vs2);
    std::cout << std::boolalpha << (vd_vs1 == vd_vs2) << std::endl;
    std::cout << vd_vs1.ToStr() << std::endl;
    std::cout << vd_vs2.ToStr() << std::endl;

    tdata::str_t si("^i123$^i456$^i67842134$");
    tdata::int_t ii;
    std::cout << si << std::endl;
    for (size_t i = 0; i < si.size() && tdata::tdata_traits<tdata::int_t>::FromStr(ii, si, &i); )
    {
        std::cout << ii << std::endl;
    }

    tdata::str_t sr("^r0.000000$^r1.000000$^r2.000000$^r0.0000032$");
    tdata::real_t rr;
    std::cout << sr << std::endl;
    for (size_t i = 0; i < sr.size() && tdata::tdata_traits<tdata::real_t>::FromStr(rr, sr, &i); )
    {
        std::cout << rr << std::endl;
    }

    tdata::str_t ss(R"K(^s^1:2\$$^s^3:4\$$^s^5:6\$$^s^7:8\$$^s^9:10\$$)K");
    tdata::str_t sss;
    std::cout << ss << std::endl;
    for (size_t i = 0; i < ss.size() && tdata::tdata_traits<tdata::str_t>::FromStr(sss, ss, &i); )
    {
        std::cout << sss << std::endl;
    }

    tdata::str_t vsi(R"K(^I0$^I3:1:2:3$)K");
    tdata::vint_t vii;
    std::cout << vsi << std::endl;
    for (size_t i = 0; i < si.size() && tdata::tdata_traits<tdata::vint_t>::FromStr(vii, vsi, &i); )
    {
        std::copy(vii.begin(), vii.end(), std::ostream_iterator<tdata::int_t>(std::cout, " "));
        std::endl(std::cout);
        vii.clear();
    }

    tdata::str_t vsr(R"K(^R0$^R6:1.100000:2.200000:3.300000:4.400000:5.500000:6.600000$)K");
    tdata::vreal_t vrr;
    std::cout << vsr << std::endl;
    for (size_t i = 0; i < si.size() && tdata::tdata_traits<tdata::vreal_t>::FromStr(vrr, vsr, &i); )
    {
        std::copy(vrr.begin(), vrr.end(), std::ostream_iterator<tdata::real_t>(std::cout, " "));
        std::endl(std::cout);
        vrr.clear();
    }

    tdata::str_t vss(R"K(^S0$^S3:^1\:2\$:^3\:4\$:^5\:6\$$)K");
    tdata::vstr_t vsss;
    std::cout << vss << std::endl;
    for (size_t i = 0; i < vss.size() && tdata::tdata_traits<tdata::vstr_t>::FromStr(vsss, vss, &i); )
    {
        std::copy(vsss.begin(), vsss.end(), std::ostream_iterator<tdata::str_t>(std::cout, " "));
        std::endl(std::cout);
        vsss.clear();
    }

    std::cout << "============================================" << std::endl;

    auto ks = d_i8.ToStr() + d_i16.ToStr() + d_i32.ToStr() + d_i64.ToStr()
            + d_u8.ToStr() + d_u16.ToStr() + d_u32.ToStr() + d_u64.ToStr()
            + d_f.ToStr() + d_d.ToStr()
            + d_str.ToStr()
            + d_pc.ToStr() + d_cpc.ToStr() + d_pcc.ToStr()
            + vd_vi1.ToStr() + vd_vi2.ToStr()
            + vd_vr1.ToStr() + vd_vr2.ToStr()
            + vd_vs1.ToStr() + vd_vs2.ToStr();
    std::cout << ks << std::endl;
    tdata::str_t::size_type pos = 0;
    do
    {
        tdata::TData data;
        if (!tdata::TData::FromStr(data, ks, &pos))
        {
            break;
        }
        std::cout << data.ToStr() << std::endl;
    } while (pos <= ks.size());

    return 0;
}

