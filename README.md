<!--
FILE: README.md
-->

# Mighty Eagle C

Mighty Eagle C is a C implementation of the Mighty Eagle template engine.

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

```console
pomake.exe MightyEagleLib32.ppj
pomake.exe MightyEagleTest32.ppj
pomake.exe MightyEagleDemo32.ppj
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
