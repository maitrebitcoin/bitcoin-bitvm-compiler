#pragma once

#include <string>
#include <vector>
#include <assert.h>
#include "Type.h"
#include "VariableDefinition.h"
class BuildContext;

// incomplet type at compilz time, we only have the name
class IncompleteType : public Type {
protected:
	// name of the type
	std::string name;
public:
	// constructor
	IncompleteType(std::string n) : name(n) {}

	// type is a basic type (ie not a struct) ?
	virtual bool is_basic(void) const  override { return false; }
	// type is complete type (false : only the name is known)
	virtual bool is_complete(void) const  override { return false; }
	// return the size in bits of the type
	virtual int size_in_bit(void) const override { assert(false); return 0; }
	// compare types
	virtual bool is_same_type(const Type& other) const override { assert(false); return false; }
	// get type name
	virtual std::string	get_name(void) const override { return name; }
	// get a copy of the type
	virtual Type* get_copy(void) const override { assert(false); return nullptr; } // non copiable

};


// usert type structure
class TypeStruct : public Type {
public:
	// name of the struct
	std::string name;
	// meberes of the struct
	std::vector<VariableDefinition> members;

	// constructor
	TypeStruct(std::string name) : name(name) {}

	// type is a basic type (ie not a struct) ?
	virtual bool is_basic(void) const { return false; }
	// type is complete type (false : only the name is known)
	virtual bool is_complete(void) const  override { return true; }
	// return the size in bits of the type
	virtual int size_in_bit(void) const override;
	// compare types
	virtual bool is_same_type(const Type& other) const override;
	// cast to "TypeStruct"
	virtual const TypeStruct* cast_to_TypeStruct(void) const override { return this; }
	// get type name
	virtual std::string	get_name(void) const override {return name	;}
	// get the member by name
	struct Member {
		bool is_valid = false;
		int index = 0;
		Type* type = nullptr;
		int offest_in_bit = 0;
	};
	Member get_member_by_name(const std::string& name) const;
	// get a copy of the type
	virtual Type* get_copy(void) const override { return new TypeStruct(*this); }


};
