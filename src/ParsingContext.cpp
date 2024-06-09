// context durent parsing, 
// to help toke identification anad keep line numbers for error messages

#include "ParsingContext.h"
#include "Compiler.h"
#include "TokenId.h"
#include "Compiler.h"

// called when '{' is found
void ParsingContext::open_bracket(void) {
	opened_bracket++;
	in_body = true;
	in_set_var_possible = true;
}
void ParsingContext::close_bracket(void) {
	opened_bracket--;
	in_body = (opened_bracket > 0);
}

// caled for a new line 
void ParsingContext::on_new_line(void) {
	num_line++;
}

// caled for each new token 
void ParsingContext::on_new_token(const CToken& token, const CLexer& lexer)
{
	// bracket counting
	if (token.type == '{')
		open_bracket();
	if (token.type == '}') {
		close_bracket();
		in_declare_struct = false;
	}
	// parenthesis counting
	if (token.type == '(')
		opened_parenthesis++;
	if (token.type == ')')
		opened_parenthesis--;


	if (token.type == TOKEN_KEYWORKD_STRUCT)
		in_declare_struct = true;
	if (token.type == TOKEN_USE_STRUCT)
		in_use_struct = true;
	else if (token.type != '.' && token.type != TOKEN_USE_STRUCT_MEMBER)
		in_use_struct = false;

	// in function declaration
	if (!in_body)
	{
		if (token.type == '(')
			in_fn_param = true;
		if (token.type == ')')
			in_fn_param = false;
	}
	// in function body
	if (in_body)
	{
		// in declation of type in body
		if (is_token_type(token.type)) // frorm TOKEN_TYPE_BOOL to TOKEN_TYPE_UINT256
		{
			in_decl_localvar    = true;
			in_set_var_possible = false;
		}
		// for "bool b=a" case :
		if (token.type == TOKEN_IDENTIFIER_DECL) {
			in_decl_localvar   = false;
		}
		// end of statement
		if (token.type == ';') {
			in_decl_localvar    = false;
			in_set_var_possible = true;
		}
		if (    (token.type != ';')  // after a statement
			 && (token.type != '{') // begin of block
			 && (token.type != '}')) // afeter end of block. ex; "if (a) {}\nint8 b=3;"
	    {
			// any token afer ';' or '{' cannot be a affecation
			in_set_var_possible = false;
		}
		// for () : enterind ans exiting
		if (token.type == TOKEN_KEYWORKD_FOR)
			in_for_statement = true;
		if (in_for_statement && token.type == ')' && opened_parenthesis==0)
			in_for_statement = false;


	}

	// to avoid confustion between a and a[2]
	next_token_type = 0;
	if (token.type == TOKEN_IDENTIFIER)	{
		CToken next_token = lexer.get_next_token_const();
		next_token_type = next_token.type;
	}
	
			
}
