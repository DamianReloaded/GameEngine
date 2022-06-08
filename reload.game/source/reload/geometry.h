#pragma once
namespace reload {
    template <typename T>
    struct point
    {
        T x;
        T y;
    };

    template <typename T>
    struct size
    {
        T width;
        T height;
    };

    template <typename T>
    struct rect
    {
        reload::point<T> location;
        reload::size<T>  size;
    };

    #define lib_reload_game_typedef(name, type) namespace name { typedef reload::point<type> point; typedef reload::size<type> size; typedef reload::rect<type>  rect; }
    lib_reload_game_typedef(i8, int8_t);
    lib_reload_game_typedef(u8, uint8_t);
    lib_reload_game_typedef(i16, int16_t);
    lib_reload_game_typedef(u16, uint16_t);
    lib_reload_game_typedef(i32, int32_t);
    lib_reload_game_typedef(u32, uint32_t);
    lib_reload_game_typedef(i64, int64_t);
    lib_reload_game_typedef(u64, uint64_t);
    lib_reload_game_typedef(flt, float);
    lib_reload_game_typedef(dbl, double);
    #undef lib_reload_game_typedef
}

