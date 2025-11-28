# ModernCLI / mcli

<p align="center">
	<img src="docs/assets/mcli-logo.svg" alt="ModernCLI logo" width="220" />
</p>

<p align="center">
	<a href="https://github.com/damiandrzewicz/modern-cli/actions/workflows/ci.yml">
		<img src="https://github.com/damiandrzewicz/modern-cli/actions/workflows/ci.yml/badge.svg?branch=main" alt="CI status" />
	</a>
	<a href="https://opensource.org/licenses/MIT">
		<img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License: MIT" />
	</a>
</p>

ModernCLI (`mcli`) is a modern, header-first C++23 command-line framework.

It focuses on:

- **Type-safe arguments** – options are parsed directly into strongly-typed values.
- **Declarative APIs** – describe your CLI once, let the library handle parsing and help.
- **Modern C++** – constexpr-friendly design, no macros-heavy interface, clean integration with tooling.

> Status: Experimental / early-stage. The core API and internals are still evolving.

## Features (planned / in progress)

- Header-first library with a small, focused core.
- Declarative CLI definition (flags, options, positional arguments).
- Strongly-typed argument parsing.
- Automatic help / usage generation.
- Extensible design for subcommands and configuration files.