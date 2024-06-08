#pragma once

#include <string>
#include <vector>
#include <assert.h>
#include "Type.h"


// array on a type
class TypeArray : public Type {
public:
	// type on each element
	Type *element_type;
	// number of elements
	int size;

	// constructor
	TypeArray(Type* type, int arary_size);
	// cast to "TypeArray"
	virtual const TypeArray* cast_to_TypeArray(void) const { return this; }

	// type is a basic type (ie not a struct) ?
	virtual bool is_basic(void) const { return false; }
	// type is complete type (false : only the name is known)
	virtual bool is_complete(void) const  override { return true; }
	// return the size in bits of the type
	virtual int size_in_bit(void) const override;
	// compare types
	virtual bool is_same_type(const Type& other) const override;
	// get type name
	virtual std::string	get_name(void) const override;
	// get the member by name
	struct Member {
		bool is_valid = false;
		int index = 0;
		Type* type = nullptr;
		int offest_in_bit = 0;
	};
	Member get_member_by_name(const std::string& name) const;
	// get a copy of the type
	virtual Type* get_copy(void) const override;


};
