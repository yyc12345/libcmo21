# BMap Bindings Generation

A helper program to generate BMap binding in Python and CSharp

This program is consisted by 2 parts, first is Python part which extract BMap interface function declarations from source file, thus following Antlr step can focus on syntax analyze.  
Next part is Antlr part. It analyse extracted function declarations and output corresponding Python binding and CSharp binding.
