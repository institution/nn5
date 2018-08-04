#include "randm.hpp"

namespace ext {
	std::default_random_engine e{std::random_device()()};
}
