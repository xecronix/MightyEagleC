FILE: 40_repo_layout_and_conventions.md
# Mighty Eagle C - Repo Layout And Conventions

## Status

This document defines the intended folder structure and naming conventions for the Mighty Eagle C project.

Conventions matter for this project.

## Root Layout

Current intended project layout:

`C:\dev\MightyEagleC\`

Expected major contents:

- root source files for the library
- `demo/`
- `docs/`
- `output/`
- `release/`
- `src/`
- `test/`

## Scenario File Conventions

The `test/data/` folder should use consistent scenario naming.

Each scenario should share a common base name.

Example:

- `parse_substitute_tag.tpl`
- `parse_substitute_tag.res`
- `parse_substitute_tag.dat`

This convention matters because helper logic depends on matching base names.

## Header Placement Rules

### Library headers
Library headers belong in:
- `src/include/`

### Demo headers
Demo-specific headers belong in:
- `demo/src/include/`

### Test headers
Test-specific headers belong in:
- `test/src/include/`

### Release headers
Public headers intended for packaged release belong in:
- `release/include/`
