#pragma once

#include <cstdint>

/* Type definitions */
typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;
typedef signed long long	int64;

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

#define BYTE uint8
#define WORD uint16
#define DWORD uint32
#define QWORD uint64
#define FLAG bool
#define RAW_VALUE char
#define to_raw(ptr) (reinterpret_cast<RAW_VALUE*>(ptr))
#define to_byte(ptr) (reinterpret_cast<BYTE*>(ptr))

class RAW_DATA
{
public:
	RAW_VALUE* data;
	const size_t length;

	RAW_DATA() = delete;
	RAW_DATA(const size_t length);
	~RAW_DATA();

	__forceinline operator BYTE*() { return reinterpret_cast<BYTE*>(data); }
	__forceinline operator int8*() { return reinterpret_cast<int8*>(data); }
	__forceinline operator RAW_VALUE*() { return data; }

	__forceinline RAW_VALUE& operator[](uint32 idx) { return data[idx]; }


};

template<typename T>
__forceinline T max(T t0, T t1) { return t0 >= t1 ? t0 : t1; }

template<typename T>
__forceinline T min(T t0, T t1) { return t0 <= t1 ? t0 : t1; }
