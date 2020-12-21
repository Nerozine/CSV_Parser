#include <iostream>
#include <tuple>
#include <fstream>
#include <type_traits>


template <int idx, typename... Args>
typename std::enable_if <(idx >= sizeof...(Args)), void>::type printTupleByIdx(std::ostream& os, std::tuple<Args...> const& t) { }

template <int idx, typename... Args>
typename std::enable_if <(idx < sizeof...(Args)), void>::type printTupleByIdx(std::ostream& os, std::tuple<Args...> const& t) {
	if (idx != 0) {
		os << ",";
	}
	os << std::get<idx>(t);
	printTupleByIdx<idx + 1>(os, t);
}

template <typename... Args>
std::ostream& operator<<(std::ostream& os, std::tuple<Args...> const& t) {
	printTupleByIdx<0>(os, t);
	return os;
}