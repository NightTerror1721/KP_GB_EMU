#pragma once

#include "raw_memory.h"
#include <iostream>

using address_t = uint16_t;

struct ByteWordListener : public ReadWriteListener<byte_t>, public ReadWriteListener<word_t>
{
	virtual byte_t operator() (const byte_t& base) const override { return base; };
	virtual word_t operator() (const word_t& base) const override { return base; };

	virtual void operator() (byte_t& base, const byte_t& value) override { base = value; };
	virtual void operator() (word_t& base, const word_t& value) override { base = value; };
};

template<class _Ty = byte_t>
class AddressSide
{
public:
	using type_t = _Ty;
	using allocator_t = ReadWriteAllocator<type_t>;
	using const_allocator_t = ReadOnlyAllocator<type_t>;
	using ptr_t = AccessorPointer<type_t>;
	using const_ptr_t = ConstAccessorPointer<type_t>;
	using listener_t = ReadWriteListener<type_t>;

	AddressSide(listener_t listener) : _listener(listener) {}

	allocator_t operator* () { return operator[](0); }
	const_allocator_t operator* () const { return operator[](0); }

	virtual void reset() = 0;

	virtual void onMmuWrite(const address_t& address, const byte_t& value) {}
	virtual void onMmuWrite(const address_t& address, const word_t& value) {}

	virtual allocator_t operator[] (const size_t& offset) = 0;
	virtual const_allocator_t operator[] (const size_t& offset) const = 0;

	virtual ptr_t operator& () = 0;
	virtual const_ptr_t operator& () const = 0;

protected:
	listener_t _listener;
};

class InvalidAddressSide : public AddressSide<>
{
private:
	word_t _value[1];

public:
	InvalidAddressSide() : AddressSide(ByteWordListener()) {}

	allocator_t operator[] (const size_t& offset) override { _value[0] = 0; return { &_listener, *reinterpret_cast<byte_t*>(_value) }; }
	const_allocator_t operator[] (const size_t& offset) const override { return { &_listener, *reinterpret_cast<const byte_t*>(_value) }; }

	ptr_t operator& () override { return { &_listener, _value }; }
	const_ptr_t operator& () const override { return { &_listener, _value }; };

	void reset() {}
};

template<class _Ty = byte_t>
class DefaultMemory : public AddressSide<_Ty>
{
public:
	using AddressSide::allocator_t;
	using AddressSide::const_allocator_t;
	using AddressSide::ptr_t;
	using AddressSide::const_ptr_t;
	using AddressSide::listener_t;

protected:
	_Ty* const _mem;
	const size_t _size;
	const bool _free;

public:
	DefaultMemory(listener_t listener, const size_t& size, bool freeMemory = true) : AddressSide<_Ty>(listener), _size(size), _mem(new _Ty[_size]), _free(freeMemory) {}
	DefaultMemory(listener_t listener, _Ty* const& ptr, const size_t& size, bool freeMemory) : AddressSide<_Ty>(listener), _size(size), _mem(ptr), _free(freeMemory) {}
	~DefaultMemory() { freeMemory(); }

	void dump(const unsigned int& columns = 0x10)
	{
		_Ty* p = _mem;

		std::cout << std::hex;
		for (size_t i = 0; i < _size; p++, i++)
		{
			if (!(i % columns) && i)
				std::cout << std::endl;
			std::cout << static_cast<unsigned int>(*p);
		}
		std::cout << std::dec << std::endl;
	}

	virtual void reset() override
	{
		if(_free)
			std::memset(_mem, 0, sizeof(_Ty) * _size);
	}

	virtual allocator_t operator[] (const size_t& offset) override { return { _mem[offset] }; }
	virtual const_allocator_t operator[] (const size_t& offset) const override { return { _mem[offset] }; }

	virtual ptr_t operator& () override { return { _mem }; }
	virtual const_ptr_t operator& () const override { return { _mem }; };

protected:
	virtual void freeMemory()
	{
		if(_free)
			delete[] _mem;
	}
};

class RAM : public DefaultMemory<>
{
public:
	RAM(const size_t& size);
	RAM(const RAM& base); //shadow ram
};
