FILE: 10_library_spec.md
# Mighty Eagle C - Library Spec

## Status

This spec reflects the intended behavior of Mighty Eagle after clarifying design intent and correcting known bugs and quirks from the historical Euphoria implementation.

This document defines the behavior of the static library product used by both `MightyEagleDemo32.exe` and `MightyEagleTest32.exe`.

Where historical code differs from this document, this document should be treated as the intended behavior.

## Purpose

Mighty Eagle is a text templating engine.

It parses a template string and produces an output string by:

- replacing substitution tags with data values
- optionally invoking substitution callbacks for custom replacement logic
- invoking action callbacks that receive a sub-template and may expand it into repeated or transformed output
- supporting nested tags inside action blocks

Primary use cases include:

- generic text templating
- report generation
- translation from flat or relational data into hierarchical text output
- batch file or makefile generation
- HTML generation
- recursive or repeated template expansion

## Canonical Error Handling Rule

If a tag is syntactically well-formed but cannot be resolved because data or callback handling is unavailable, the original template text should be preserved in the output.

If a tag is malformed, parsing should stop and return an error code.

This rule applies equally to substitution tags and action tags.

## Tag Families

There are 2 tag families.

### 1. Substitution Tags

Syntax:

`{=tag:}`

Examples:

- `{=name:}`
- `{= name:}`
- `{=name :}`
- `{= name :}`

### 2. Action Tags

Syntax:

`{@action_name ... :}`

Example:

`{@sayit2x {=cnt:}. IT
:}`

## Valid Tag Name Characters

The only valid tag-name characters are:

`._abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`

Spaces are not valid tag-name characters.

## Resolution Rules

### Substitution tag resolution order

1. If a substitution callback is registered for that tag, call it.
2. If the callback returns a string, emit that string.
3. If the callback returns no replacement, continue normal resolution.
4. If the callback returns an error, stop parsing and return that error.
5. If the tag exists in input data, emit the value.
6. If the tag does not exist in input data, emit the original substitution tag text unchanged.

### Action tag resolution order

1. If an action callback is registered for that action tag, call it.
2. If the callback returns a string, emit that string.
3. If the callback returns no replacement, emit the original full action tag unchanged.
4. If the callback returns an error, stop parsing and return that error.
5. If no callback exists, emit the original full action tag unchanged.

## Malformed Tag Rules

Malformed tags must stop parsing and return an error code.

Examples:

- `{=tag}` because `:}` is required
- `{=tag: ` because closing `}` is missing
- `{=first name:}` because internal space is not a valid tag-name character
- `{=:}` because tag name is missing
- `{@:}` because action name is missing
- any tag that reaches end-of-input without a required closing sequence

## Behavior With Non-Tags

A `{` character is treated as the start of a tag only if the next character is:

- `=`
- `@`

Otherwise, the `{` and following text are emitted literally.

## Nested Parsing Pattern

Action callbacks are expected to call parse recursively on their sub-template when expansion of nested content is desired.

This is a core design pattern of the engine.

## Historical Bug Fix Decisions

### 1. Default Error logic
When a tag is well-formed but missing data or callbacks, echo the tag in it's entirety.
When a tag is not well formed raise an error.

### 2. Unknown action callback fallback
Historical partial-output behavior is a bug.

Correct behavior:
- preserve the original full action tag text unchanged

### 3. Internal spaces in substitution names
Historical permissive parsing is a bug.

Correct behavior:
- internal spaces inside tag names are malformed and must return an error code

### 4. Unclosed tags at end-of-input
Historical behavior that echoed unfinished tag text is a bug.

Correct behavior:
- return an error code for malformed syntax

## Regression Checklist

1. Template with no tags returns unchanged text.
2. Substitution tags resolve against the data map.
3. Missing substitution data preserves the original tag text.
4. Registered substitution callbacks can transform output.
5. Callback errors propagate out.
6. Unknown action callback preserves the original full action tag text.
7. Invalid empty action tag name returns an error code.
8. Internal spaces inside substitution tags return an error code.
9. Unclosed substitution tags return an error code.
10. Unclosed action tags return an error code.
11. Nested action callbacks work.
