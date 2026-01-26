# Developer Notes

This article tells the details of this project for the developer of this project.

## Java

### Java

Before configuring Java, you should know that all Java parts of this project are just for using Antlr.
Although Antlr provide C#, C++ and etc bindings, but it is written in Java.
So using Java can completely utilize Antlr, and I can easily get corresponding examples.

Under this premise, you will know the reason why all these project's Java code are written in crude layout.
There is no Maven, no Gradle and anything like that in this project.
We just use the most primitive way to build Java code: use Java compiler to compile all Java files into class files, and order Java runtime to execute the main class.
Because I just want my Java code works. There is no any other expectations.

Also based on this premise, you can use your favorite IDE to process these code freely.
The only restriction is that you should not break this premise: use primitive way to build these Java code.

After understanding this premise, you now can configure Java.
The JDK I used is OpenJDK 21.
It would be okey for you to use any JDK you like, but obviouslly JDK 8 is NOT recommended.

### Antlr

Antlr is used in this project to generate parser and lexer.
After configuring Java, you can configure Antlr.
Antlr official document will instruct you how to configure Antlr, ideally.
Basically, after configuring Antlr, you should make sure that there is 2 programs named `antlr4` and `grun` in your environment.
So that you can correctly execute commands provided in each projects.

The Antlr I used is Antlr 4.13.0.
It would be okey for you to use any Antlr you like.

### Gson

Some Java code use Google Gson as its dependency.
The Gson versio I used is Gson 2.10.1.
It would be okey for you to use any Antlr you like.

## Python

For most Python code written in this project, we use Astral UV to manage them.
Astral UV is a great tool to manage Python code.
Before developing with Python code provided in this project, please prepare Astral UV first.
