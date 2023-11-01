#pragma once

#include <string>
#include <vector>
#include "Type.h"
#include "VariableDefinition.h"

// usert type structure
class TypeStruct : public Type {
public:
	// name of the struct
	std::string name;
	// meberes of the struct
	std::vector<VariableDefinition> members;

	// type is a basic type (ie not a struct) ?
	virtual bool is_basic(void) const { return false; }
	// return the size in bits of the type
	virtual int size_in_bit(void) const override;
	// compare types
	virtual bool is_same_type(const Type& other) const override;
	// cast to "TypeStruct"
	virtual const TypeStruct* cast_to_TypeStruct(void) const override { return this; }
};
