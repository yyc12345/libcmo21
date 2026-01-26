# Enums Migration

A helper program to migrate existing Virtools enum declarations into other formats.

Original Virtools SDK have various enum declarations. All of them are defined as C format and their formation are not uniform. This sub-project will use laxer and parser to recognize these diverse declarations, extract them as a series of uniform Java data struct and output them as C++ code (as C++ enum class syntax for LibCmo using), Python code (for PyBMap using), C# code (for BMapSharp using) and Rust code (for rusty-bmap using).

The steps processing existing enum declaration is called migration as this sub-project name told.

## Usage

This project is consisting of two parts: Enums Analyzer and Enums Render

### Setup Environment

First we stay at the root directory of this project (this README file located).
And execute `set ENUMS_MIGRATION_ROOT=$(pwd)` on POSIX-like OS, or `set ENUMS_MIGRATION_ROOT=%CD%` on Windows, to set environment variable.
This environment variable will be used by Enums Analyzer and Enums Render later.

### Enums Analyzer

We should first run Eunms Analyzer to recognize existing enum declarations.
And export them as JSON file which will be utilized by Enums Render.

#### Build

Enter `EnumsAnalyzer` directory, and execute following command to generate Antlr lexer and parser:

```
antlr4 CKGeneralLexer.g4
antlr4 CKEnumsParser.g4
antlr4 CKDefinesParser.g4
```

Keep staying that directory, and execute following command to build Java code.

```
javac *.java
```

#### Run

Keep staying this directory, and execute following command to run program.

```
java MainRunner
```

After running, program will process input file located in `Input`directory, and output JSON file to `Intermediate` directory.

### Enums Render

* Enter `EnumsRender` directory and setup it witl Astral UV.
* Execute `uv run main.py` to run program.
* Program will process JSON file located in `Intermediate` directory, and output final artifacts to `Output` directory.
