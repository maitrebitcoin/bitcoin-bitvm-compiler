// context durent parsing, 
// to help toke identification anad keep line numbers for error messages

#include "ParsingContext.h"
#include "Compiler.h"
#include "TokenId.h"

// caled for a new line 
void ParsingContext::on_new_line(void) {
	num_line++;
}

// caled for each new token 
void ParsingContext::on_new_token(const CToken& token)
{
	if (token.type == '{')
		open_bracket();
	if (token.type == '}') {
		close_bracket();
		in_declare_struct = false;
	}
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
		else	{
			// any token afer ';' cannot be a affecation
			in_set_var_possible = false;
		}
	}
}
