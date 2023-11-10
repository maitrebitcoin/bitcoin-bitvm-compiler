#pragma once

#include <assert.h>
#include <string>
class TypeStruct;
class TypeBasic;

// type of a variable or returned by a function. 
//ex : "bool" ou "struct MyStruct"
class Type {
public:
	enum class Native {
		undefined,	// not set/ invalide
		bit,		// bool
		int8,		// signed 8 bits integer
		uint8,		// unsigned 8 bits integer
		int32,		// signed 32 bits integer
		uint32,		// 
		int64,		// signed 64 bits integer
		uint64,		// 
		int256,		// igned 256 bits integer / hash
		uint256,	// 
		user_struct,// user defined structure type
	};

	// return the size in bits of the type
	virtual int size_in_bit(void) const = 0;
	// compare
	virtual bool is_same_type(const Type& other) const = 0;
	// type is a basic type (ie not a struct) ?
	virtual bool is_basic(void) const = 0;
	// type is complete type (false : only the name is known)
	virtual bool is_complete(void) const = 0;
	// type is bool ?
	virtual bool is_bool(void) const { return false; }
	// type is integer ?
	virtual bool is_integer(void) const { return false; }
	// get type name
	virtual std::string	get_name(void) const = 0;
	// get a copy of the type
	virtual Type* get_copy(void) const = 0;

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
	// type is complete type (false : only the name is known)
	virtual bool is_complete(void) const { return true; }
	// compare
	virtual bool is_same_type(const Type& other) const override;
	// get type name
	virtual std::string	get_name(void) const override;
	// get a copy of the type
	virtual Type* get_copy(void) const override { return new TypeBasic(*this); }

	// get the native type
	Native get_native_type(void) const { return native_type; }
};

