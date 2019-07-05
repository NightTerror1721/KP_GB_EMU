#pragma once

#include <cinttypes>

using byte_t = uint8_t;
using word_t = uint16_t;

template<class _Ty>
struct ReadWriteListener
{
	virtual _Ty operator() (const _Ty& base) const { return base; };
	virtual void operator() (_Ty& base, const _Ty& value) { base = value; };
};

using ByteReadWriteListener = ReadWriteListener<byte_t>;
using WordReadWriteListener = ReadWriteListener<word_t>;



template<class _Ty>
class ReadOnlyAllocator
{
private:
	const ReadWriteListener<_Ty>* const _lst;
	const _Ty& _base;

public:
	ReadOnlyAllocator(const ReadWriteListener<_Ty>* const& listener, const _Ty& base) : _lst(listener), _base(base) {}

	ReadOnlyAllocator() = delete;
	ReadOnlyAllocator(ReadOnlyAllocator&&) = delete;

	ReadOnlyAllocator& operator= (ReadOnlyAllocator&&) = delete;

	operator const _Ty() const { return _lst->operator()(_base); }
};

template<class _Ty>
class ReadWriteAllocator
{
private:
	ReadWriteListener<_Ty>* const _lst;
	_Ty& _base;

public:
	ReadWriteAllocator(ReadWriteListener<_Ty>* const& listener, _Ty& base) : _lst(listener), _base(base) {}

	ReadWriteAllocator() = delete;
	ReadWriteAllocator(ReadWriteAllocator&&) = delete;

	ReadWriteAllocator& operator= (ReadWriteAllocator&&) = delete;

	ReadWriteAllocator& operator= (const _Ty& value) { _lst->operator()(_base, value); return *this; }
	operator const _Ty() const { return _lst->operator()(_base); }
};





template<class _Ty, class _Base>
class BaseAccessorPointer;


template<class _Ty>
using AccessorPointer = BaseAccessorPointer<_Ty, byte_t>;

using ByteAccessorPointer = AccessorPointer<byte_t>;
using WordAccessorPointer = AccessorPointer<word_t>;


template<class _Ty>
using ConstAccessorPointer = const BaseAccessorPointer<_Ty, const byte_t>;

using ConstByteAccessorPointer = ConstAccessorPointer<byte_t>;
using ConstWordAccessorPointer = ConstAccessorPointer<word_t>;




template<class _Ty, class _Base>
class BaseAccessorPointer
{
public:
	using base_t = _Base;
	using type_t = _Ty;
	using r_alloc = const ReadOnlyAllocator<_Ty>;
	using rw_alloc = ReadWriteAllocator<_Ty>;
	using listener_t = ReadWriteListener<_Ty>;

private:
	listener_t* const _lst;
	base_t* _ptr;

public:
	BaseAccessorPointer(listener_t* const& listener, void* const& ptr) : _lst(listener), _ptr(reinterpret_cast<base_t>(ptr)) {}
	BaseAccessorPointer(const listener_t* const& listener, const void* const& ptr) : _lst(listener), _ptr(reinterpret_cast<base_t>(ptr)) {}

	BaseAccessorPointer& operator= (const size_t& offset) { _ptr = offset; return *this; }
	BaseAccessorPointer& operator+= (const size_t& offset) { _ptr += offset; return *this; }
	BaseAccessorPointer& operator-= (const size_t& offset) { _ptr -= offset; return *this; }
	BaseAccessorPointer& operator*= (const size_t& offset) { _ptr *= offset; return *this; }
	BaseAccessorPointer& operator/= (const size_t& offset) { _ptr /= offset; return *this; }

	BaseAccessorPointer operator+ (const size_t& offset) const { return { _ptr + offset }; }
	BaseAccessorPointer operator- (const size_t& offset) const { return { _ptr - offset }; }
	BaseAccessorPointer operator* (const size_t& offset) const { return { _ptr * offset }; }
	BaseAccessorPointer operator/ (const size_t& offset) const { return { _ptr / offset }; }

	BaseAccessorPointer& operator++ () { ++_ptr; return *this; }
	BaseAccessorPointer operator++ (int) { base_t* ptr = _ptr++; return { ptr }; }
	BaseAccessorPointer& operator-- () { --_ptr; return *this; }
	BaseAccessorPointer operator-- (int) { base_t* ptr = _ptr--; return { ptr }; }

	bool operator== (const BaseAccessorPointer& o) const { return _ptr == o._ptr; }
	bool operator!= (const BaseAccessorPointer& o) const { return _ptr != o._ptr; }
	bool operator> (const BaseAccessorPointer& o) const { return _ptr > o._ptr; }
	bool operator< (const BaseAccessorPointer& o) const { return _ptr < o._ptr; }
	bool operator>= (const BaseAccessorPointer& o) const { return _ptr >= o._ptr; }
	bool operator<= (const BaseAccessorPointer& o) const { return _ptr <= o._ptr; }

	bool operator! () { return !_ptr; }
	operator bool() { return _ptr; }

	rw_alloc operator* () { return { _lst, *reinterpret_cast<_Ty*>(_ptr) }; }
	r_alloc operator* () const { return { _lst, *reinterpret_cast<_Ty*>(_ptr) }; }

	rw_alloc operator[] (const size_t& offset) { return { _lst, *reinterpret_cast<_Ty*>(_ptr + offset) }; }
	r_alloc operator[] (const size_t& offset) const { return { _lst, *reinterpret_cast<_Ty*>(_ptr + offset) }; }

	template<class _Ty2>
	AccessorPointer<_Ty2> as() { return { reinterpret_cast<ReadWriteListener<_Ty2>*>(_lst), _ptr }; }

	template<class _Ty2>
	ConstAccessorPointer<_Ty2> as() const { return { reinterpret_cast<ReadWriteListener<_Ty2>*>(_lst), _ptr }; }

	inline ByteAccessorPointer as_byte() { return this->as<byte_t>(); }
	inline WordAccessorPointer as_word() { return this->as<word_t>(); }
	inline ConstByteAccessorPointer as_byte() const { return this->as<byte_t>(); }
	inline ConstWordAccessorPointer as_word() const { return this->as<word_t>(); }
};
