#pragma once

#include <string>

// compiler error
class Error {
public:
	std::string file_name;
	std::string function_name;
	int line_number = 0;
	// main error message
	std::string message;
public:
	// constructor
	Error(void) {}
	Error(const char* mess) : message(mess) {}
	Error(const char* mess, std::string more_info) : message(mess) { message += ' ' + more_info; }

	// format the error message
	std::string toString(void) const {
		std::string res;
		if (function_name != "")
			res += function_name + "() :\n";
		if (file_name != "")
			res += file_name + "(" + std::to_string(line_number) + ") : " + message;
		else
			res += message;
		return res;
	}
};