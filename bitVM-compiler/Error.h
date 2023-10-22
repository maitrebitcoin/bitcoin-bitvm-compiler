#pragma once

// compiler error
class Error {
public:
	// line number
	int line_number = 0;
	// error message
	std::string message;
public:
	// constructor
	Error(void) {}
	Error(const char* mess) : message(mess) {}
	Error(const char* mess, const char* more_info) : message(mess) { message += ' ' + more_info; }
};