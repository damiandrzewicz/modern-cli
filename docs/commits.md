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