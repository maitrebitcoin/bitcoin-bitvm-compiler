#pragma once

#include <string>
class Type;

// interface for input correspondance
class InterfaceInputsMap {
public:
	struct Info {
		bool found= false;
		int index=0;
		int offset_in_bit=0;
		Type* type=nullptr;
	};
	// get a parameter info by name
	virtual Info find_info_by_name(std::string name) const = 0;
};