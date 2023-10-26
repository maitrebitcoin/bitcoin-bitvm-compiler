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
	if (token.type == '}')
		close_bracket();

	if (!in_body)
	{
		if (token.type == '(')
			in_fn_param = true;
		if (token.type == ')')
			in_fn_param = false;
	}
	if (in_body)
	{
		// in declation of type in body
		if (  (token.type == TOKEN_TYPE_BOOL)
			||(token.type == TOKEN_TYPE_BYTE)) {
			in_decl_localvar = true;
		}
		if (token.type == ';')
			in_decl_localvar = false;
	}
}
