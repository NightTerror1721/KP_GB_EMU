#pragma once

#include <cstdint>

/* Type definitions */
using int8	 = signed char;
using int16  = signed short int;
using int32  = signed int;
using int64  = signed long long int;

using uint8  = unsigned char;
using uint16 = unsigned short int;
using uint32 = unsigned int;
using uint64 = unsigned long long int;

using __byte = char;
using __bit  = bool;

using uint   = unsigned int;

#define BYTE uint8
#define WORD uint16
#define DWORD uint32
#define QWORD uint64
#define FLAG __bit
#define RAW_VALUE __byte
#define to_raw(ptr) (reinterpret_cast<RAW_VALUE*>(ptr))
#define to_byte(ptr) (reinterpret_cast<BYTE*>(ptr))

class __bytes
{
public:
	RAW_VALUE* data;
	const size_t length;

	__bytes() = delete;
	__bytes(const size_t length);
	~__bytes();

	__forceinline operator BYTE*() { return reinterpret_cast<BYTE*>(data); }
	__forceinline operator int8*() { return reinterpret_cast<int8*>(data); }
	__forceinline operator RAW_VALUE*() { return data; }

	__forceinline RAW_VALUE& operator[](DWORD idx) { return data[idx]; }


};
#define RAW_DATA __bytes


template<typename T>
__forceinline T max(T t0, T t1) { return t0 >= t1 ? t0 : t1; }

template<typename T>
__forceinline T min(T t0, T t1) { return t0 <= t1 ? t0 : t1; }
