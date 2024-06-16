// type of a variable or function. 

#include "Type.h"
#include "Error.h"


// return the size in bits of a type
int TypeBasic::size_in_bit(void) const
{
	switch (native_type)
	{
	case Type::Native::undefined:
		assert(false);
		return 0;
	case Type::Native::bit:    return   1;
	case Type::Native::int8:   return   8;
	case Type::Native::uint8:  return   8;
	case Type::Native::int32:  return  32;
	case Type::Native::uint32: return  32;
	case Type::Native::int64:  return  64;
	case Type::Native::uint64: return  64;
	case Type::Native::int256: return 256;
	case Type::Native::uint256:return 256;
	default:
		assert(false);
		return 0;
	}
}

// type is integer ?
bool TypeBasic::is_integer(void) const  
{ 
	switch (native_type)
	{
	case Type::Native::undefined:
		assert(false);
		return false;
	case Type::Native::bit:    
		return   false;
	case Type::Native::int8:   
	case Type::Native::uint8:  
	case Type::Native::int32:  
	case Type::Native::uint32: 
	case Type::Native::int64:  
	case Type::Native::uint64: 
	case Type::Native::int256: 
	case Type::Native::uint256:
		return true;
	default:
		assert(false);
		return 0;
	}

}

// get a TypeBasic for a requested nb of bits
TypeBasic TypeBasic::get_TypeBasic_for_bitsize(int nb_bits)
{
	switch (nb_bits)
	{
	case 1:  return TypeBasic(Type::Native::bit);
	case 8:  return TypeBasic(Type::Native::int8);
	case 32: return TypeBasic(Type::Native::int32);
	case 64: return TypeBasic(Type::Native::int64);
	case 256:return TypeBasic(Type::Native::int256);
	default:
		assert(false);
		throw Error("No type avalaide for nb of bit : ", std::to_string(nb_bits)  );
		//return TypeBasic(Type::Native::undefined);
	}
};



// compare
 bool TypeBasic::is_same_type(const Type& other) const {
	// must be a basic type
	const TypeBasic* other_basic = other.cast_to_TypeBasic();
	if (other_basic == nullptr)
		return false;
	return native_type == other_basic->native_type;
}

 // get type name
 std::string TypeBasic::get_name(void) const {
	 switch (native_type)
	 {
	 case Type::Native::undefined:
	 default:
		 assert(false);
		 return "unknown";
	 case Type::Native::bit:      return "bool";
	 case Type::Native::int8:     return "int8";
	 case Type::Native::uint8:    return "uint8";
	 case Type::Native::int32:    return "int32";
	 case Type::Native::uint32:   return "uint32";
	 case Type::Native::int64:    return "int64";
	 case Type::Native::uint64:   return "uint64";
	 case Type::Native::int256:   return "int256";
	 case Type::Native::uint256:  return "uint256";
	 }
 }

