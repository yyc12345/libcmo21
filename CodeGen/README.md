# Code Gen

A helper program to generate some definations.

```
antlr4 CKGeneralLexer.g4
antlr4 CKEnumParser.g4
antlr4 CKErrorParser.g4
antlr4 CKClassidParser.g4

javac CK*.java
java CKMainGen
```
