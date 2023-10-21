// Implentation of the BtcContract class and its sub-classes

#include "BtcContract.h"
#include <assert.h>


// return the size in bits of a type
int Type::size_in_bit(void) const
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

// return the number of bits needed for the input parameters
int Function::size_in_bit_input(void) const
{
	// addd the size of each parameter
	int nb_bit = 0;
	for (Parameter param_i : definition.parameters)
		nb_bit += param_i.type.size_in_bit();
	return nb_bit;
}
// return the number of bits needed to store the return value
int Function::size_in_bit_output(void) const {
	// size of the return type
	return definition.return_type.size_in_bit();
}

// Definition constructor
Function::Definition::Definition(Type type, std::string function_name)
	: return_type(type)
	, name(function_name)
{
	assert(function_name.size() > 0);
}

