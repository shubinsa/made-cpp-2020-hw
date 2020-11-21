#ifndef MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H
#define MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H

#include "xor_sum.h"
#include "binary.h"

template <size_t Head, size_t... Tail>
struct GetBiggest {
	static constexpr size_t value = Head > GetBiggest<Tail...>::value ? Head : GetBiggest<Tail...>::value;
};

template <size_t Head>
struct GetBiggest<Head> {
	static constexpr size_t value = Head;
};

template <size_t count, size_t d, size_t Head, size_t... Tail>
struct GetIndex {
	static constexpr ssize_t value = GetBit<d, Head>::value ? count : GetIndex<count + 1, d, Tail...>::value;
};

template <size_t count, size_t d, size_t Head>
struct GetIndex<count, d, Head> {
	static constexpr ssize_t value = GetBit<d, Head>::value ? count : -1;
};

template <size_t... Heaps>
struct Game {
	static constexpr size_t s = XorSum<Heaps...>::value;
	static constexpr bool first_player_wins = s;
	static constexpr size_t d = Log2<s>::value - 1;
	static constexpr size_t biggest = GetBiggest<Heaps...>::value;
	static constexpr ssize_t k = GetIndex<0, d, Heaps...>::value;
	static constexpr ssize_t first_move_heap_index = k;
	static constexpr ssize_t first_move_rocks_count = biggest - (s ^ biggest);
};


#endif /// MEANWHILE_IN_THE_SEVENTH_GALAXY_FIRST_VARIANT_GAME_H.