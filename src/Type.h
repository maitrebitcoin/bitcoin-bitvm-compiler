#pragma once


// type of a variable or function. 
//ex : "bool"
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
protected:
	Native native_type = Native::undefined;
public:
	// constructor
	Type(void) {}
	Type(Native t) : native_type(t) {}
	Type(const Type& source) : native_type(source.native_type) {}

	// return the size in bits of the type
	virtual int size_in_bit(void) const;
	// type is defineds ?
	bool is_defined(void) const { return native_type != Native::undefined; }
	// type is bool ?
	bool is_bool(void) const { return native_type == Native::bit; }
	// type is integer ?
	bool is_integer(void) const { return native_type == Native::int8 || native_type == Native::uint8; }
	// type is a basic type (ie not a struct) ?
	bool is_basic(void) const { return native_type != Native::user_struct && is_defined(); }
	// compare
	bool is_same_type(const Type& other) const {
		return native_type == other.native_type;
	}
	Native get_native_type(void) const { return native_type; }

	// cast to "TypeStruct"
	virtual class TypeStruct* cast_to_TypeStruct(void)  { return nullptr; }
};

