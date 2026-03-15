FILE: 20_demo_spec.md
# Mighty Eagle C - Demo Spec

## Status

This document defines the functional requirements for the Mighty Eagle C demo.

This spec is rebuilt from the historical demo source.

## Purpose

The demo exists to prove that the Mighty Eagle library can be used to generate real structured output from nested template callbacks.

The historical demo generates an HTML sales report from fake customer and order data.

That behavior should be preserved in spirit in the C version.

## Output

Primary executable:
- `MightyEagleDemo32.exe`

## Dependency Rule

The demo must use the Mighty Eagle static library.

It must not contain a duplicate parser implementation.

## Primary Demo Goal

The demo must prove these library capabilities in one working example:

1. Action callbacks can be registered and invoked.
2. Nested action callbacks work.
3. Substitution tags work inside action-generated output.
4. Recursive parsing through callback-driven sub-templates works.
5. The library can be used to produce a realistic text artifact, not just toy output.

## Historical Demo Behavior

The historical Euphoria demo does the following:

1. Creates a Mighty Eagle instance.
2. Registers 2 action callbacks:
   - `customer_cb`
   - `order_cb`
3. Builds an in-memory HTML template string.
4. Uses fake customer and order data.
5. Calls parse once at the top level.
6. Uses nested callback-driven parsing to expand the final HTML.
7. Prints the final HTML to standard output.

## Minimum Acceptance Criteria

The demo is considered acceptable when all of the following are true:

1. It builds as `MightyEagleDemo32.exe`.
2. It links against the Mighty Eagle static library.
3. It creates and uses a Mighty Eagle instance.
4. It registers at least 2 action callbacks.
5. It performs nested callback-driven parsing.
6. It produces HTML text.
7. The output contains customer data and related order rows.
8. The parser logic is performed by the library, not duplicated inside the demo.
