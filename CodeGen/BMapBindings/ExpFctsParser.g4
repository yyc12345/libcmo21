parser grammar ExpFctsParser;
options { tokenVocab = ExpFctsLexer; }

program: fctDecl* ;

fctDecl
    : EXPFCTS_EXPORT varType EXPFCTS_IDENTIFIER '(' (fctArg (',' fctArg)*)? ')' ';'
    ;

fctArg
    : EXPFCTS_FILE_DECL '(' EXPFCTS_IDENTIFIER ')' # fctArgFileDecl
    | EXPFCTS_MESHTRANS_DECL '(' EXPFCTS_IDENTIFIER ')' #fctArgMeshTransDecl
    | EXPFCTS_OBJECT_DECL '(' EXPFCTS_IDENTIFIER ',' EXPFCTS_IDENTIFIER ')' # fctArgObjDecl
    | EXPFCTS_PARAM_IN '(' varType ',' EXPFCTS_IDENTIFIER ')' # fctArgParamIn
    | EXPFCTS_PARAM_OUT '(' varType ',' EXPFCTS_IDENTIFIER ')' # fctArgParamOut
    ;

varType
    : EXPFCTS_IDENTIFIER ('::' EXPFCTS_IDENTIFIER)* '*'? 
    ;

