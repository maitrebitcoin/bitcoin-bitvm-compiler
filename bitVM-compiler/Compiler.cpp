// implémentation of the compiler

#include "Compiler.h"
#include <sstream>
#include <vector>
#include <assert.h>
#include <regex>




// type of a variable or function
class Type {
public:
	enum class Native {
		undefined,	// not set/ invalide
		bit,		// bool
		int8,		// char
		uint8,		// byte / unsigned char
		int64,		// long long
		uint64,		// unsigned long long
		uint256,	// unsigned __int256
	};
protected:
	Native native_type = Native::undefined;
public:
	// return the size in bits of the type
	int size_in_bit(void) const;

};
// return the size in bits of the type
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

class Operand;
// Math expression. ex :"a+2"
class Expression {
protected:
	// type of the result
	Type result_type;
	// opération
	enum class Operation {
		op_and,
		op_or,
		op_not,
	};
	Operation operation;
	// left operand
	Operand* left_operand;
	// right operand
	Operand* right_operand;
	// expression as  string for debug purposes
	std::string expression_for_debug;
};

// base class for a statement. 
// ex "v=a+2" or "return a+2;"
class StatementBase {
protected:

};
// "return" statement
class Statement_Return : public StatementBase {
protected:
	// expression to return
	Expression expression;

};



// represents a function, ex : bool main(bool a, bool b) { return a & b; }
class Function {
protected:
	// name of the function
	std::string name;
	// return type of the function
	Type return_type;
	// parameters of the function
	struct Parameter {
		Type type;
		std::string name;
	};
	std::vector<Parameter> parameters;

public:
	// return the number of bits needed for the input parameters
	int size_in_bit_input(void) const;
	// return the number of bits needed to store the return value
	int size_in_bit_output(void) const;

};
// return the number of bits needed for the input parameters
int Function::size_in_bit_input(void) const
{
	// addd the size of each parameter
	int nb_bit = 0;
	for (Parameter param_i: parameters)
		nb_bit += param_i.type.size_in_bit();
	return nb_bit;
}
// return the number of bits needed to store the return value
int Function::size_in_bit_output(void) const {
	// size of the return type
	return return_type.size_in_bit();
}



// compile a memory stream
bool Compiler::compile(std::istream& source_code, Error& error_out)
{
	// read the source code
	static const int max_line_size = 1024;
	char line_raw[max_line_size];
	int cureent_line_number = 0;
	while (source_code.getline(line_raw, max_line_size)) {
		// compile 1 line
		std::string source_code_at_line(line_raw);
		try {
			_compile_line(source_code_at_line);
		}
		catch (Error& compiler_error)	{
			// compilation failed
			error_out = compiler_error;
			error_out.message += " : " + source_code_at_line; // std::to_string(cureent_line_number);
			error_out.line_number = cureent_line_number;
			return false;
		}
		// next line
		cureent_line_number++;
	}
	// OK
	return true;
}



struct TokenDefinition {
	int token_type;
	const char* token_value;
	const char* regex;
};

#define TOKEN_TYPE_BOOL  257
#define TOKEN_TYPE_BYTE  258
#define TOKEN_IDENTIFIER 258
#define TOKEN_NUMBER	 259
TokenDefinition token_definition[] =
{
	{ TOKEN_TYPE_BOOL,  "bool"},
	{ TOKEN_TYPE_BYTE,  "byte"},
	{ TOKEN_IDENTIFIER, nullptr, "[a-zA-a_][a-zA-a0-9_]*"},
	{ TOKEN_NUMBER,		nullptr, "[0-9]*"},
};


// get 1 token from the line
CToken Compiler::_get_next_token(std::string& code_in_out) {

	// ignore white space and tab
	while (code_in_out[0] == ' ' || code_in_out[0] == '\t')
		code_in_out.erase(0, 1);

	// if the string is empty, return an empty token
	if (code_in_out.size() == 0)
		return CToken(0, "");

	// check if the string star wiht of of the known token
	for (TokenDefinition definition : token_definition) {
		// ex "bool"
		if (definition.token_value != nullptr) {
			// check if the string start with the token
			if (code_in_out.compare(0, strlen(definition.token_value), definition.token_value) == 0) {
				// remove the token from the string
				code_in_out.erase(0, strlen(definition.token_value));
				// return the token
				return CToken(definition.token_type, definition.token_value);
			}
		}
		// ex "[0-9*]"
		else {
			assert(definition.regex != nullptr);
			//  check if the string start with the token with definition.regex
			std::string regex_definition = definition.regex;
			std::regex regex_i( regex_definition );
			std::smatch regex_match;
			if (std::regex_search(code_in_out , regex_match, regex_i)) {
				std::string match_string = regex_match[0];
				// remove the token from the string
				code_in_out.erase(0, match_string.size() );
				// return the token
				return CToken(definition.token_type, match_string);
			}
		}
	} // for each token definition

	// send the first char as a token
	char c = code_in_out[0];
	char tok_c[] = { c,0 };
	code_in_out.erase(0, 1);
	return CToken(c, tok_c);
}

// compile 1 line of code
void Compiler::_compile_line(std::string line_of_code) {
	// ignore empty lines
	if (line_of_code.size() == 0) return;



	// read all the tokens
	while (1) {
		// get a token for the line
		CToken token = _get_next_token(line_of_code);
		if (token.type == 0)
			return;
		// add to stack	
		token_stack.push_back(token);


	}

		
	// TODO : parse the line
	throw Error("syntax error");
}
