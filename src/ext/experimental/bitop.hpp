#pragma once



template <class T>
int8_t get_first_bit_set_pos(T a) {
	auto one = a & ~(a-1);
	
	int8_t i = 0;
	while (one >>= 1) ++i;
	return i;
}
