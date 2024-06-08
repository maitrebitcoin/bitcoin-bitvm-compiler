
#include "TypeArray.h"

// constructor
TypeArray::TypeArray( Type* type, int array_size)
	: element_type(type)
	, size(array_size) {
	assert(element_type != nullptr);
	assert(size >= 1);
}

// compare types
bool TypeArray::is_same_type(const Type& other) const {
	// other must be a array
	const TypeArray* other_array = other.cast_to_TypeArray();
	if (other_array == nullptr)
		return false;
	// compare type and size
	return element_type->is_same_type(*other_array->element_type)
		&& size == other_array->size;
}

// return the size in bits of the type
int TypeArray::size_in_bit(void) const {
	return element_type->size_in_bit() * size;
}

// get type name
// ex : "int[10]"
std::string	TypeArray::get_name(void) const
{
	std::string name;
	name = element_type->get_name() + "[" + std::to_string(size) + "]";
	return name; 
}

// get a copy of the type
Type* TypeArray::get_copy(void) const 
{
	return new TypeArray(element_type, size);
}
