SOURCE(fast_list.cpp) fast_list
HEADER(fast_list.h)

PUBLIC(
    fast_list CLASS(FastList) {
        METHOD(add)
        METHOD(get)
        METHOD(size)
        METHOD(clear)
        FIELD(data)
    }

    fast_list FUNC(fast_sort)
    fast_list FUNC(fast_reverse)
    fast_list FUNC(fast_sum)
    fast_list FUNC(fast_max)
    fast_list FUNC(fast_min)
)
