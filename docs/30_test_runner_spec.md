FILE: 30_test_runner_spec.md
# Mighty Eagle C - Test Runner Spec

## Status

This document defines the functional requirements for the Mighty Eagle C test runner.

## Purpose

The test runner exists to verify that the Mighty Eagle static library behaves according to the Mighty Eagle specs.

The test runner is expected to:

- load test scenarios
- parse template input using the library
- compare actual output against golden output
- report pass or fail results

## Output

Primary executable:
- `MightyEagleTest32.exe`

## Dependency Rule

The test runner must use the Mighty Eagle static library.

It must not contain a second parser implementation.

## Test Model

The test runner uses a scenario-driven test model.

Each test scenario is based on test data files stored under:

- `test/data/`

Each scenario may use the following file types:

- `.tpl` = template input
- `.res` = expected rendered output or expected error marker
- `.dat` = input data

## Data File Format

Each line is:

`key||value`

## Equality Rule

The primary test mechanism is equality comparison.

Pass rule:
- actual output exactly equals expected output

Fail rule:
- actual output does not exactly equal expected output

For error scenarios, actual error code must exactly match expected error code.

## Scenario Discovery

For the initial version, explicit test listing is preferred because it is simpler and easier to debug.

## Console Output Expectations

The runner should print enough information for a human to understand:

- which scenario is running
- whether the scenario passed
- whether the scenario failed
- what the final summary is

## Process Exit Code

- return `0` if all tests pass
- return nonzero if any test fails or if the runner encounters a fatal setup error
