# Vector Generator

## Overview

Vector types (LibCmo::VxMath::VxVector3 and etc) and Vector-like types (LibCmo::VxMath::VxColor and etc) nearly have similar declaration except slight differences (basically is the count of factors).
Manually writing these declarations is boring and easy to cause potential invisible bugs.
So we use a Python script to generate these declarations batchly to prevent any defects indroduced above.

## Usage

* Setup this project with Astral UV.
* Execute `uv run main.py`
* Generated files are `VxTypes.hpp` and `VxTypes.cpp` and locate in this directory.
