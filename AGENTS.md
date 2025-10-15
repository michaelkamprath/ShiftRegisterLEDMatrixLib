# ShiftRegisterLEDMatrixLib Agent Instructions
This library provides a generalized API to create and drive an image on LED matrix where shift registers, such as the 74HC595 or DM13a, are used to control the rows and columns of the matrix. It is designed to be use with the Arduino framework on a number of microcontroller platforms. A detailed description of this library and its use cases can be found in `README.md`.

## Scope
These instructions apply to the entire repository unless a nested `AGENTS.md` overrides them.

## Code style
- Match the surrounding formatting when editing C++ sources; the project primarily uses tabs for indentation inside functions and spaces for alignment.
- Avoid introducing `try`/`catch` blocks around includes and keep the code portable across supported Arduino cores.
- Prefer existing helper classes (for example `FspTimer`, `TimerThree`, etc.) instead of pulling in new dependencies when adding platform support.

## Documentation
- Update `README.md`, `CHANGELOG.md`, and relevant files under `extras/` whenever you add support for a new board or change user-facing behaviour.
- When documenting commands, use fenced code blocks with a language hint (e.g., ` ```sh `) so they render correctly on GitHub.

## Testing
- There is no automated test suite; note "tests not run" in the final summary when you do not execute hardware tests.
