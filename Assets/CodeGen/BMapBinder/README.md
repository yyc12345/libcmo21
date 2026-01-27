# BMap Binder

A helper program to generate BMap binding for Python, C# and Rust.

## Usage

This program is consisted by 3 parts: ExpFcts Extractor, ExpFcts Analyzer and ExpFcts Render.

"ExpFcts" is stand for "Exported Functions".

### Setup Environment

First we stay at the root directory of this project (this README file located).
And execute `set BMAP_BINDER_ROOT=$(pwd)` on POSIX-like OS, or `set BMAP_BINDER_ROOT=%CD%` on Windows, to set environment variable.
This environment variable will be used later by 3 parts conststing this project.

Then we navigate to the root directory of this repository (where you can find top-level `CMakeLists.txt` file). And execute `set LIBCMO21_REPO_ROOT=$(pwd)` on POSIX-like OS, or `set LIBCMO21_REPO_ROOT=%CD%` on Windows, to set environment variable.
This environment variable also will be used later.

### ExpFcts Extractor

We should first run ExpFcts Extractor to extract BMap exported function declarations fron header file.
BMap header file include various contents, not only the declaration of exported functions, but also many of other utilities.
We use regex to extract BMap exported function declarations only to avoid build a complete C lexer and parser in following steps and make our work easier.

For running this program, please following these steps:

* Enter `ExpFctsExtractor` directory and setup it with Astral UV.
* Execute `uv run main.py` to run program.
* Program will process BMap header file `BMExports.hpp` from BMap project located in root directory of this repository, and output extracted text to `Extracted` directory with name `BMExports.hpp`.

### ExpFcts Analyzer

Now we can run ExpFcts Analyzer to analyze extracted BMap exported function declarations.

#### Build

Enter `ExpFctsAnalyzer` directory, and execute following command to generate Antlr lexer and parser:

```
antlr4 ExpFctsLexer.g4
antlr4 ExpFctsParser.g4
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

After running, program will process input file `BMExports.hpp` located in `Extracted` directory, and output JSON file to `Analyzed` directory with name `BMExports.json`.

### ExpFcts Render

* Enter `ExpFctsRender` directory and setup it with Astral UV.
* Execute `uv run main.py` to run program.
* Program will process JSON file `BMExports.json` located in `Analyzed` directory, and output final artifacts to `Output` directory.
