FILE: 00_project_overview.md
# Mighty Eagle C - Project Overview

## Status

This document defines the high level shape of the Mighty Eagle C project.

## Purpose

Mighty Eagle C is a C implementation of the Mighty Eagle template engine.

The project has 3 primary outputs:

1. A static library containing the Mighty Eagle template engine.
2. A demo executable that uses the library.
3. A test runner executable that uses the library.

## Main Deliverables

### 1. Static Library

Primary output:
- `MightyEagleLib32.lib`
- `MightyEagleLib64.lib` 

Purpose:
- Contains the main Mighty Eagle template engine implementation.
- Provides the API used by the demo executable.
- Provides the API used by the test runner executable.

The static library is the main product.

### 2. Demo Executable

Primary output:
- `MightyEagleDemo32.exe`

Purpose:
- Demonstrates real use of the Mighty Eagle library.
- Proves that the library can be consumed by a normal application.
- Exercises important engine features through a practical example.

The demo must use the static library to do its work.
It must not contain a duplicate parser implementation.

### 3. Test Runner Executable

Primary output:
- `MightyEagleTest32.exe`

Purpose:
- Validates engine behavior.
- Loads scenario files and compares actual rendered output against golden output.
- Provides a repeatable way to confirm that the library preserves intended behavior.

The test runner must use the static library to do its work.
It must not contain a duplicate parser implementation.

## Document Set

- `00_project_overview.md`
- `10_library_spec.md`
- `20_demo_spec.md`
- `30_test_runner_spec.md`
- `40_repo_layout_and_conventions.md`

## Summary

Mighty Eagle C is a 3-output project:

- 1 static library
- 1 demo executable
- 1 test runner executable

The library is the main product.
The demo proves the library is usable.
The test runner proves the library is correct.
