# Commit message conventions

We use a Conventional Commit–style format:

`<type>(optional-scope): <short description>`

- `<type>` is one of:
  - `feat`      – new feature or capability
  - `fix`       – bug fix (user-visible behavior was wrong)
  - `docs`      – documentation-only changes
  - `chore`     – maintenance / housekeeping
  - `refactor`  – code changes that neither fix bugs nor add features
  - `test`      – add or improve tests
  - `ci`        – CI or automation changes
  - `build`     – build tooling, deps, packaging
  - `perf`      – performance improvements
  - `style`     – formatting / style-only changes

- `<short description>`:
  - Must start with a **lowercase** letter.
  - Is a short, imperative phrase (e.g. `add flag validator`, `fix parser bug`).

Examples:

- `feat: add short flag support`
- `fix(parser): handle duplicate options`
- `docs(readme): document fluent flag API`
- `style: reformat core headers with clang-format`

## Reverts

Reverts are allowed, but they must still follow the same Conventional Commit–style header.

Accepted format:

`revert(optional-scope): <short description>`

Guidelines:

- Use `revert` as the type.
- Keep the short description **lowercase**.
- Include the reverted commit subject in quotes when helpful.
- If the revert is for a specific area, add a scope (e.g. `revert(parser): ...`).

Examples:

- `revert: "feat: add short flag support"`
- `revert(parser): revert "fix(parser): handle duplicate options"`