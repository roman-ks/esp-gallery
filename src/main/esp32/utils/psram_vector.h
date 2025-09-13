#include <vector>
#include "psram_allocator.h"

template <typename T>
using psram_vector = std::vector<T, PsramAllocator<T>>;