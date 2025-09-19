#include <stdlib.h>
#include "esp32-hal-psram.h"
#include "../log.h"

template <typename T>
class PsramAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    template <class U> struct rebind { using other = PsramAllocator<U>; };

    PsramAllocator() = default;
    template <class U>
    constexpr PsramAllocator(const PsramAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        // LOGF_D("PSRAM: Size: %d, free: %d, min free %d, max alloc %d , desired size: %d\n", 
        //     ESP.getPsramSize(), ESP.getFreePsram(), ESP.getMinFreePsram(), ESP.getMaxAllocPsram(), n * sizeof(T));
        void* p = ps_malloc(n * sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) noexcept {
        free(p);
    }
};


// Required so different PsramAllocators compare equal
template <class T, class U>
bool operator==(const PsramAllocator<T>&, const PsramAllocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const PsramAllocator<T>&, const PsramAllocator<U>&) { return false; }