#pragma once

// Compiler class definition

#include <string>
#include <vector>

class CToken {
public:
	// type of the token. ex '=' or TOKEN_TYPE_BOOL
	int type;
	// value of the token. ex "bool"
	std::string value;
public:
	// constructor
	CToken(int t, std::string v) : type(t), value(v) {}

};

class Compiler
{
public:
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
	};
protected:
	// tokenss waiting to be processed by the compiler
	std::vector<CToken> token_stack;
public:
	// compile a memory stream
	bool compile(std::istream& source_code, Error& error_out);
protected:
	// compile 1 line of code
	void _compile_line(std::string line_of_code);
	// get 1 token from code
	CToken _get_next_token(std::string& code_in_out);

};


