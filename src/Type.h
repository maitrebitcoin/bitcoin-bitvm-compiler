#pragma once

#include <assert.h>
class TypeStruct;
class TypeBasic;

// type of a variable or returned by a function. 
//ex : "bool" ou "struct MyStruct"
class Type {
public:
	enum class Native {
		undefined,	// not set/ invalide
		bit,		// bool
		int8,		// char  signe char
		uint8,		// byte / unsigned char
		int64,		// long long
		uint64,		// unsigned long long
		uint256,	// unsigned __int256
		user_struct,// user defined structure type
	};

	// return the size in bits of the type
	virtual int size_in_bit(void) const = 0;
	// compare
	virtual bool is_same_type(const Type& other) const = 0;
	// type is a basic type (ie not a struct) ?
	virtual bool is_basic(void) const = 0;
	// type is bool ?
	virtual bool is_bool(void) const { return false; }
	// type is integer ?
	virtual bool is_integer(void) const { return false; }

	// cast to "TypeBasic"
	virtual const TypeBasic* cast_to_TypeBasic(void) const { return nullptr; }
	// cast to "TypeStruct"
	virtual const TypeStruct* cast_to_TypeStruct(void) const { return nullptr; }

};
 
// basic/native/scalar types.
//ex : "bool"
class TypeBasic : public Type {


protected:
	Native native_type = Native::undefined;
public:
	// constructor
	TypeBasic(void) {}
	TypeBasic(Native t) : native_type(t) { assert(t != Native::user_struct); }
	TypeBasic(const TypeBasic& source) : native_type(source.native_type) {}
	// cast to "TypeBasic"
	virtual const TypeBasic* cast_to_TypeBasic(void) const override { return this; }

	// return the size in bits of the type
	virtual int size_in_bit(void) const override;
	// type is bool ?
	virtual bool is_bool(void) const  override { return native_type == Native::bit; }
	// type is integer ?
	virtual bool is_integer(void) const override { return native_type == Native::int8 || native_type == Native::uint8; }
	// type is a basic type (ie not a struct) ?
	virtual bool is_basic(void) const { return true; }
	// compare
	virtual bool is_same_type(const Type& other) const override;

	// get the native type
	Native get_native_type(void) const { return native_type; }
};

