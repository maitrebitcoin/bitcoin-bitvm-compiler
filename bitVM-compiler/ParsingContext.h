#pragma once

class CToken;

//  parsing context
class ParsingContext {
protected:
	bool in_body = false;
	bool in_fn_param = false;
	bool in_decl_localvar = false;
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
		in_body = true;
	}
	void close_bracket(void) {
		in_body = true;
	}
};
