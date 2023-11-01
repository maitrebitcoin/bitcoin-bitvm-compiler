// type of a variable or function. 

#include "Type.h"


// return the size in bits of a type
int TypeBasic::size_in_bit(void) const
{
	switch (native_type)
	{
	case Type::Native::undefined:
		assert(false);
		return 0;
	case Type::Native::bit:    return 1;
	case Type::Native::int8:   return 8;
	case Type::Native::uint8:  return 8;
	case Type::Native::int64:  return 64;
	case Type::Native::uint64: return 64;
	case Type::Native::uint256:return 256;
	default:
		assert(false);
		return 0;
	}
}
// compare
 bool TypeBasic::is_same_type(const Type& other) const {
	// must be a basic type
	const TypeBasic* other_basic = other.cast_to_TypeBasic();
	if (other_basic == nullptr)
		return false;
	return native_type == other_basic->native_type;
}