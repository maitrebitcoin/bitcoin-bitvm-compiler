#pragma once

class CToken;
class CLexer;

//  parsing context
class ParsingContext {
public:
	int  opened_bracket = 0; // number of opened bracket
	int  opened_parenthesis = 0; // number of opened parenthesis
	bool in_body = false;
	bool in_declare_struct = false; // in a struct declaration
	bool in_use_struct = false; // in struct.membeer
	bool in_fn_param = false;
	bool in_decl_localvar = false;
	bool in_set_var_possible = false;
	bool in_for_statement = false; // in the for (a,b,c) statement
	int next_token_type = 0;
	int num_line = 0;
public:
	// caled for each new token 
	void on_new_token(const CToken& token, const CLexer& lexer);
	// caled for a new line 
	void on_new_line(void);
	// get current line number
	int get_current_line_number(void) const { return num_line; }

protected:
	// called when '{' is found
	void open_bracket(void);
	void close_bracket(void);
};
