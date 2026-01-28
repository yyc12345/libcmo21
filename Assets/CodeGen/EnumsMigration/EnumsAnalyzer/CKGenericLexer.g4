lexer grammar CKGenericLexer;

channels { COMMENTS, WHITESPACE }

// keywords
CKGENERIC_TYPEDEF: 'typedef' ;
CKGENERIC_DEFINE: '#define' ;
CKGENERIC_ENUM: 'enum' ;
// symbols
CKGENERIC_LBRACKET: '{' ;
CKGENERIC_RBRACKET: '}' ;
CKGENERIC_EQUAL: '=';
CKGENERIC_SEMICOLON: ';' ;
CKGENERIC_LSHIFT: '<<' ;
CKGENERIC_OR: '|' ;
CKGENERIC_COMMA: ',' ;

// identifider and number
CKGENERIC_ID: [_a-zA-Z][_a-zA-Z0-9]* ;
CKGENERIC_NUM: (('0'[xX]) | '-')? [0-9a-fA-F]+ [uUlL]* ;

// comments
CKGENERIC_LINE_COMMENT: '//' ~[\r\n]* -> channel(COMMENTS);
CKGENERIC_BLOCK_COMMENT: '/*' .*? '*/' -> channel(COMMENTS);
// whitespace
CKGENERIC_WS: [ \t\r\n]+ -> channel(WHITESPACE);
