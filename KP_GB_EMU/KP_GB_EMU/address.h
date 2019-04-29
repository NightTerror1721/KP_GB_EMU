#pragma once

#include <cinttypes>

using address_t = uint16_t;
using byte_t = uint8_t;
using word_t = uint16_t;

unsigned long long operator "" _kB(unsigned long long value);
unsigned long long operator "" _MB(unsigned long long value);

bool addr_range(const address_t& min, const address_t& current, const address_t& max);

template<class _Ty>
using ReadAdapter = _Ty(*)(const _Ty&);

template<class _Ty>
using WriteAdapter = void(*)(_Ty&, const _Ty&);

template<class _Ty>
_Ty __direct_read_adapter(const _Ty& data) { return data; }

template<class _Ty>
void __direct_write_adapter(_Ty& data, const _Ty& value) { data = value; }

template<class _Ty, class _Base, ReadAdapter<_Ty> _Read = __direct_read_adapter, WriteAdapter<_Ty> _Write = __direct_write_adapter>
class BaseAddressAccessor;

template<class _Ty, ReadAdapter<_Ty> _Read = __direct_read_adapter, WriteAdapter<_Ty> _Write = __direct_write_adapter>
using AddressAccessor = BaseAddressAccessor<_Ty, byte_t, _Read, _Write>;

template<ReadAdapter<byte_t> _Read = __direct_read_adapter, WriteAdapter<byte_t> _Write = __direct_write_adapter>
using ByteAddressAccessor = AddressAccessor<byte_t, _Read, _Write>;

template<ReadAdapter<word_t> _Read = __direct_read_adapter, WriteAdapter<word_t> _Write = __direct_write_adapter>
using WordAddressAccessor = AddressAccessor<word_t, _Read, _Write>;

template<class _Ty, ReadAdapter<_Ty> _Read = __direct_read_adapter, WriteAdapter<_Ty> _Write = __direct_write_adapter>
using ConstAddressAccessor = const BaseAddressAccessor<_Ty, const byte_t, _Read, _Write>;

template<ReadAdapter<byte_t> _Read = __direct_read_adapter, WriteAdapter<byte_t> _Write = __direct_write_adapter>
using ConstByteAddressAccessor = ConstAddressAccessor<byte_t, _Read, _Write>;

template<ReadAdapter<word_t> _Read = __direct_read_adapter, WriteAdapter<word_t> _Write = __direct_write_adapter>
using ConstWordAddressAccessor = ConstAddressAccessor<word_t, _Read, _Write>;


template<class _Ty, ReadAdapter<_Ty> _Read = __direct_read_adapter>
class ReadOnlyAllocator
{
private:
	const _Ty& _data;

public:
	ReadOnlyAllocator(const _Ty& data) : _data(data) {}

	operator _Ty() { return _Read(_data) }
	operator const _Ty() { return _Read(_data); }
};

template<class _Ty, ReadAdapter<_Ty> _Read = __direct_read_adapter, WriteAdapter<_Ty> _Write = __direct_write_adapter>
class ReadWriteAllocator
{
private:
	_Ty& _data;

public:
	ReadWriteAllocator(_Ty& data) : _data(data) {}

	ReadWriteAllocator& operator= (const _Ty& value) { _Write(_data, value); return *this; }

	operator _Ty() { return _Read(_data) }
	operator const _Ty() { return _Read(_data); }
};


template<class _Ty, class _Base, ReadAdapter<_Ty> _Read, WriteAdapter<_Ty> _Write>
class BaseAddressAccessor
{
	using r_alloc = const ReadOnlyAllocator<_Ty, _Read>;
	using rw_alloc = ReadWriteAllocator<_Ty, _Read, _Write>;

private:
	_Base* _ptr;
	

public:
	BaseAddressAccessor(void* const& ptr) : _ptr(reinterpret_cast<_Base*>(ptr)) {}
	BaseAddressAccessor(const void* const& ptr) : _ptr(reinterpret_cast<_Base*>(ptr)) {}
	BaseAddressAccessor() : _ptr(nullptr) {}

	/* Pointer manipulation */
	BaseAddressAccessor& operator= (const size_t& offset) { _ptr = offset; return *this; }
	BaseAddressAccessor& operator+= (const size_t& offset) { _ptr += offset; return *this; }
	BaseAddressAccessor& operator-= (const size_t& offset) { _ptr -= offset; return *this; }
	BaseAddressAccessor& operator*= (const size_t& offset) { _ptr *= offset; return *this; }
	BaseAddressAccessor& operator/= (const size_t& offset) { _ptr /= offset; return *this; }

	BaseAddressAccessor operator+ (const size_t& offset) const { return { _ptr + offset }; }
	BaseAddressAccessor operator- (const size_t& offset) const { return { _ptr - offset }; }
	BaseAddressAccessor operator* (const size_t& offset) const { return { _ptr * offset }; }
	BaseAddressAccessor operator/ (const size_t& offset) const { return { _ptr / offset }; }

	BaseAddressAccessor& operator++ ()    { ++_ptr; return *this; }
	BaseAddressAccessor operator++  (int) { _Base* ptr = _ptr++; return { ptr }; }
	BaseAddressAccessor& operator-- ()    { --_ptr; return *this; }
	BaseAddressAccessor operator--  (int) { _Base* ptr = _ptr--; return { ptr }; }

	/* Pointer comparison */
	bool operator== (const BaseAddressAccessor& o) const { return _ptr == o._ptr; }
	bool operator!= (const BaseAddressAccessor& o) const { return _ptr != o._ptr; }
	bool operator> (const BaseAddressAccessor& o) const { return _ptr > o._ptr; }
	bool operator< (const BaseAddressAccessor& o) const { return _ptr < o._ptr; }
	bool operator>= (const BaseAddressAccessor& o) const { return _ptr >= o._ptr; }
	bool operator<= (const BaseAddressAccessor& o) const { return _ptr <= o._ptr; }

	bool operator! () const { return !_ptr; }
	operator bool() const { return _ptr; }

	/* Read */
	operator _Ty() { return _Read(*reinterpret_cast<_Ty*>(_ptr)); }
	operator const _Ty() const { return _Read(*reinterpret_cast<const _Ty*>(_ptr)); }

	/* Write */
	BaseAddressAccessor& operator= (const _Ty& value) { _Write(*reinterpret_cast<_Ty*>(_ptr), value); return *this; }

	/* Pointer access */
	rw_alloc operator* () { return { operator _Ty() }; }
	r_alloc operator* () const { return { *reinterpret_cast<_Ty*>(_ptr) }; }

	rw_alloc operator[] (const size_t& offset) { return { *reinterpret_cast<_Ty*>(_ptr + offset) }; }
	r_alloc operator[] (const size_t& offset) const { return { *reinterpret_cast<_Ty*>(_ptr + offset) }; }

	/* Raw memory manipulation */
	_Ty* operator& () { return reinterpret_cast<_Ty*>(_ptr); }
	const _Ty* operator& () const { return reinterpret_cast<_Ty*>(_ptr); }

	
	

	ByteAddressAccessor as_byte() { return { _ptr }; }
	WordAddressAccessor as_word() { return { _ptr }; }
	ConstByteAddressAccessor as_byte() const { return { _ptr }; }
	ConstWordAddressAccessor as_word() const { return { _ptr }; }

	template<class __Type, ReadAdapter<__Type> _Read2 = __direct_read_adapter, WriteAdapter<__Type> _Write2 = __direct_write_adapter>
	AddressAccessor<__Type, _Read2, _Write2> as() { return AddressAccessor<__Type, _Read2, _Write2>(_ptr); }

	template<class __Type, ReadAdapter<__Type> _Read2 = __direct_read_adapter, WriteAdapter<__Type> _Write2 = __direct_write_adapter>
	ConstAddressAccessor<__Type, _Read2, _Write2> as() const { return ConstAddressAccessor<__Type, _Read2, _Write2>(_ptr); }
};


template<class _Ty = byte_t, ReadAdapter<_Ty> _Read = __direct_read_adapter, WriteAdapter<_Ty> _Write = __direct_write_adapter>
class AddressSide
{
public:
	using accessor_t = AddressAccessor<_Ty, _Read, _Write>;
	using const_accessor_t = ConstAddressAccessor<_Ty, _Read, _Write>;

	void writeByte(const size_t& offset, const byte_t& value);
	void writeWord(const size_t& offset, const word_t& value);

	byte_t readByte(const size_t& offset) const;
	word_t readWord(const size_t& offset) const;

	accessor_t operator* ();
	const_accessor_t operator* () const;

	virtual void reset() = 0;

	virtual void onMmuWrite(const address_t& address, const byte_t& value);
	virtual void onMmuWrite(const address_t& address, const word_t& value);

	virtual accessor_t operator[] (const size_t& offset) = 0;
	virtual const_accessor_t operator[] (const size_t& offset) const = 0;
};

class InvalidAddressSide : public AddressSide<>
{
private:
	word_t _value[1];

public:
	InvalidAddressSide() : AddressSide() {}

	accessor_t operator[] (const size_t& offset) override { _value[0] = 0; return { _value }; }
	const const_accessor_t operator[] (const size_t& offset) const override { return { const_cast<word_t*>(_value) }; }

	void reset() {}
};

class RAM : public AddressSide<>
{
private:
	byte_t* const _mem;
	const size_t _size;
	const bool _shadow;

public:
	RAM(const size_t& size);
	RAM(const RAM& base); //shadow ram
	~RAM();

	void clear();

	void dump(const unsigned int& columns = 0x10);

	void reset();

	accessor_t operator[] (const size_t& offset) override;
	const const_accessor_t operator[] (const size_t& offset) const override;
};
