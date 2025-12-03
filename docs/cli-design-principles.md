# ModernCLI — Command Line Interface Design Principles

This document defines the **opinionated CLI syntax and behavioral rules** used by ModernCLI.
It exists to keep CLI behavior **predictable, strict, ergonomic, and modern**, while avoiding
legacy Unix quirks and ambiguities.

These principles guide both the implementation inside ModernCLI, and the expectations for all
applications built using it.

---

# 1. High‑Level Philosophy

ModernCLI follows these principles:

* **One clear grammar** — predictable and simple to reason about.
* **Strict by default** — errors are explicit, no silent ignoring.
* **Type‑safe** — values are parsed into typed fields, not strings.
* **Struct‑binding first** — CLI is a transport to fill typed configuration.
* **Config‑ready** — CLI, config files, and env vars eventually share the same schema.
* **Zero legacy weirdness** — no POSIX flag clusters, no ambiguous short magic.
* **Developer‑focused UX** — predictable for scripting, automation, CI, and debugging.

ModernCLI aims for a CLI UX closer to modern tools like:

* `git` (structured subcommands)
* `kubectl` (clear hierarchy, typed flags)
* `cargo` (nice help output, consistent naming)
* `gh` (modern structure)

---

# 2. Command Structure

A CLI invocation has the following shape:

```
app [GLOBAL_OPTIONS] [SUBCOMMAND [SUBCOMMAND ...]] [OPTIONS] [POSITIONAL ...]
```

Examples:

* `app --verbose serve --port 8080 /var/www`
* `app db migrate --dry-run`
* `app user create --name John --role admin`

## 2.1 One Subcommand Path Per Invocation

ModernCLI enforces:

> **Exactly one subcommand path active per invocation.**

Examples (valid):

* `app serve`
* `app db migrate`
* `app user create`

Examples (invalid):

* `app serve backup` → two sibling subcommands → **error**

If users want to run multiple subcommands they should use:

* shell chaining (`;`, `&&`)
* or a higher‑level subcommand (e.g. `app deploy --with-backup`).

## 2.2 Nested Subcommands

Nested subcommands are allowed and encouraged when they map to clear hierarchy:

```
app remote add origin
app db migrate
app user delete john
```

Depth should remain reasonable (1–3 levels).

---

# 3. Options (Flags and Arguments)

ModernCLI recognizes two kinds of options:

* **Flags** — boolean, no value
* **Arguments** — options requiring a typed value

## 3.1 Naming

* Prefer **long options**: `--verbose`, `--port`, `--output-file`.
* Optional **short aliases**: `-v`, `-p`, `-o`.
* Only use short aliases when they are obvious and consistent.
* Use **kebab-case** exclusively: `--max-retries`, not `--max_retries`.

## 3.2 Flag Semantics

* Presence-only. If a flag appears → boolean becomes `true`.
* Do **not** accept `--flag true/false`.
* Duplicate flags → **error**:

```
app --verbose --verbose   # error: duplicate option
```

If counting semantics are ever needed (`-vvv`), they will be implemented using a **dedicated API**.

## 3.3 Argument Syntax

Two allowed forms:

* `--port 8080` (preferred)
* `--port=8080` (also accepted)

Do not accept POSIX-style compact args like:

* `-p8080`
* `--port8080`

Arguments are typed at parse time:

* `--port` → int
* `--timeout` → duration
* `--level` → enum

Invalid types produce clear errors:

```
Invalid integer for --port: "abc"
```

## 3.4 Multi‑Value Arguments

By default, repeated args are errors:

```
app --tag foo --tag bar   # error (unless multi-value declared explicitly)
```

Multi-value arguments must be explicitly declared via dedicated API (future feature):

```
cmd.multi_arg("--include", opts.includes);
```

---

# 4. Option Scoping Rules

Clear separation between global options and subcommand options:

> **Options before the first non-option token belong to the root.**
>
> **Options after a subcommand belong to that subcommand.**

Examples:

```
app --verbose serve --port 8080
# global: --verbose
# serve:  --port 8080
```

```
app serve --verbose
# serve: --verbose (not global)
```

If both scopes define an option with the same name, they are **distinct and independent**:

```
app --verbose serve --verbose
# root.verbose  = true
# serve.verbose = true
```

This dual‑verbose pattern is valid but should be used sparingly.

---

# 5. Positional Arguments

Use positional arguments for natural objects:

* paths
* filenames
* usernames
* identifiers

Examples:

```
app serve /var/www
app user delete alice
```

Rules:

* Positional arguments must be clearly documented.
* Avoid mixing positionals and options in confusing ways.
* `--` terminates option parsing:

```
app serve -- /path-with-leading-dashes
```

---

# 6. Errors and Validation

ModernCLI is intentionally strict to prevent silent bugs.

## 6.1 Unknown Options → Error

```
app serve --portt 8080
# error: unknown option '--portt' (did you mean '--port'?)
```

## 6.2 Missing Values → Error

```
app serve --port
# error: missing value for option '--port'
```

## 6.3 Duplicate Options → Error

(Default behavior unless multi-value or counter semantics are configured.)

## 6.4 Unknown Subcommands → Error

```
app serv
# error: unknown command 'serv'
# available commands: serve, build, test
```

---

# 7. Help & Discoverability

## 7.1 Help Everywhere

Every command and subcommand must support:

```
app --help
app serve --help
app user create --help
```

Printed help includes:

* usage syntax
* description
* options with types and defaults
* subcommands with one-line summaries

## 7.2 Version

Root command should support:

```
app --version
```

Output should be machine-readable:

```
app 1.2.3
```

---

# 8. Configuration Layer (Future)

ModernCLI will support structured config via:

* JSON
* YAML
* TOML
* Environment variables

Precedence:

```
defaults < config file < env vars < CLI flags
```

`.config_key()` already exists to map CLI options to config fields.

---

# 9. Exit Codes

* **0** — success
* **1** — runtime error
* **2** — CLI usage error (invalid flags, options, etc.)
* ≥ **10** — application-specific errors

This helps scripting and CI pipelines behave predictably.

---

# 10. Design Summary

ModernCLI provides a modern, clean, predictable CLI grammar:

* One active subcommand path
* Consistent `--long-options` only
* Kebab-case naming
* Typed arguments
* Strict duplicate/unknown option handling
* Clear global-vs-subcommand scoping
* Strong help output
* Struct-binding as first-class
* Future config file and env integration

This ensures that tools built on ModernCLI are:

* easy to learn
* safe to use
* simple to script
* predictable to maintain

ModernCLI is not just a parser — it is **a design philosophy for building beautiful CLIs**.
