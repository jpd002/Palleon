%{
/* need this for the call to atof() below */
#include <math.h>
#include <assert.h>
#include "QuakeShaderParser.h"
#define YY_NO_UNISTD_H
%}

%option noyywrap
%option always-interactive
%option reentrant

DIGIT    [0-9]
ID       [-*$a-zA-Z][a-zA-Z0-9/_.]*
%%

-?{DIGIT}+ {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_NUMBER, atoi(yytext)));
//	printf( "An integer: %s (%d)\n", yytext, atoi( yytext ) );
}

-?{DIGIT}?\.{DIGIT}* {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_NUMBER, atof(yytext)));
//	printf( "A float: %s (%g)\n", yytext, atof( yytext ) );
}

q3map_surfacelight|map|rgbGen|blendFunc|blendfunc|tcGen|tcgen|tcMod|tcmod|skyparms {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_KEYWORD, yytext));
//	printf( "A keyword: %s\n", yytext );
}

{ID} {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_LITERAL, yytext));
//	printf( "An identifier: %s\n", yytext );
}

"{" {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_BRACKET_OPEN));
}

"}" {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_BRACKET_CLOSE));
}

"(" {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_PARENT_OPEN));
}

")" {
	CQuakeShaderParser::TokenArray* result(reinterpret_cast<CQuakeShaderParser::TokenArray*>(yyextra));
	result->push_back(CQuakeShaderParser::TOKEN(CQuakeShaderParser::TOKEN_PARENT_CLOSE));
}

(\/\/)+[^\n]*		/* eat up comments */
[ \t\r\n]+          /* eat up whitespace */
. {
	printf( "Unrecognized character: %s\n", yytext );
	assert(0);
}
%%

CQuakeShaderParser::TokenArray CQuakeShaderParser::Tokenize(const char* script)
{
	TokenArray result;
	
	yyscan_t scanner;
	yylex_init(&scanner);

	yylex_init_extra(&result, &scanner);
    YY_BUFFER_STATE bufferState = yy_scan_string(script, scanner);
	yylex(scanner);
	yy_delete_buffer(bufferState, scanner);
	yylex_destroy(scanner);
	
	return result;
}
