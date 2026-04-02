<!--
FILE: README.md
-->

# Mighty Eagle C

Mighty Eagle C is a C implementation of the Mighty Eagle template engine.

The Mighty Eagle Templating Solution (METS) is capable of replacing a properly tagged text document with external data. This solution has the added advantage of being easily extended via the use of callbacks. Using callbacks a programmer can easily model recursive data and on the fly data translations. Some use cases include translation of data from one format to another, generic templated documents, mail templates, and other similar tasks. The program is particularly well suited for translation of relational data to hierarchical visualizations.

This repo currently contains:

- a static library build target
- a demo executable that generates nested HTML output
- a test runner executable that validates behavior using scenario files and golden output

## Current Build Outputs

- `MightyEagleLib32.lib`
- `MightyEagleDemo32.exe`
- `MightyEagleTest32.exe`

## PellesC Workspace - Easiest build method uses the IDE
MightyEagleLib32.ppw

## Build
A build.bat file is included for convenient command-line builds. If you prefer, you can also open the .ppj files located in the root, test, and demo folders in the Pelles C IDE and build them there. Bonus points if you create a single workspace in the IDE containing all three projects.

```console
./build.bat
```

## Run Tests

```console
MightyEagleTest32.exe
```

## Run Demo

```console
MightyEagleDemo32.exe
```

## Notes

- This project was rebuilt from the original Euphoria implementation and the project specs with ChatGPT assistance.
- The main behavioral rules live in `docs/10_library_spec.md`.
- The current environment validated the 32-bit static library target.
