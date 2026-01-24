# Enums Migration

A helper program to migrate existing Virtools enum declarations into other formats.

Original Virtools SDK have various enum declarations. All of them are defined as C format and their formation are not uniform. This sub-project will use laxer and parser to recognize these diverse declarations, extract them as a series of uniform Java data struct and output them as C++ code (as C++ enum class syntax for LibCmo using), Python code (for PyBMap using), and C# code (for BMapSharp using).

The steps processing existing enum declaration is called migration as this sub-project name told.

```
antlr4 CKGeneralLexer.g4
antlr4 CKEnumsParser.g4
antlr4 CKDefinesParser.g4

javac *.java
java MainRunner
```
