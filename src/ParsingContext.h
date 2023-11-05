#pragma once

class CToken;

//  parsing context
class ParsingContext {
protected:
	int  opened_bracket = 0;
	bool in_body = false;
	bool in_declare_struct = false; // in a struct declaration
	bool in_use_struct = false; // in struct.membeer
	bool in_fn_param = false;
	bool in_decl_localvar = false;
	bool in_set_var_possible = false;
	int num_line = 0;
public:
	// caled for each new token 
	void on_new_token(const CToken& token);
	// caled for a new line 
	void on_new_line(void);
	// get current line number
	int get_current_line_number(void) const { return num_line; }

protected:
	// called when '{' is found
	void open_bracket(void) {
		opened_bracket++;
		in_body = true;
		in_set_var_possible= true;
	}
	void close_bracket(void) {
		opened_bracket--;
		in_body = (opened_bracket > 0);
	}
};
