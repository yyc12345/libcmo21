lexer grammar CKGeneralLexer;

channels { COMMENTS, WHITESPACE }

// keywords
CKGENERAL_TYPEDEF: 'typedef' ;
CKGENERAL_DEFINE: '#define' ;
CKGENERAL_ENUM: 'enum' ;
// symbols
CKGENERAL_LBRACKET: '{' ;
CKGENERAL_RBRACKET: '}' ;
CKGENERAL_EQUAL: '=';
CKGENERAL_SEMICOLON: ';' ;
CKGENERAL_LSHIFT: '<<' ;
CKGENERAL_OR: '|' ;
CKGENERAL_COMMA: ',' ;

// identifider and number
CKGENERAL_ID: [_a-zA-Z][_a-zA-Z0-9]* ;
CKGENERAL_NUM: (('0'[xX]) | '-')? [0-9a-fA-F]+ [uUlL]* ;

// comments
CKGENERAL_LINE_COMMENT: '//' ~[\r\n]* -> channel(COMMENTS);
CKGENERAL_BLOCK_COMMENT: '/*' .*? '*/' -> channel(COMMENTS);
// whitespace
CKGENERAL_WS: [ \t\r\n]+ -> channel(WHITESPACE);
