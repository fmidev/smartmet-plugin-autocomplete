# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

SmartMet autocomplete plugin (`smartmet-plugin-autocomplete`) — a SmartMet Server plugin that provides type-ahead location search with optional weather forecast data. It serves JSON responses at the `/autocomplete` HTTP endpoint.

## Build commands

```bash
make                # Build autocomplete.so
make test           # Run integration tests (requires geonames DB — see below)
make format         # Run clang-format on all source and test files
make clean          # Clean build artifacts
make rpm            # Build RPM package
```

## Testing

Tests are **integration tests**, not unit tests. They use `smartmet-plugin-test` to spin up a SmartMet reactor with the plugin loaded and replay HTTP requests against it.

```bash
cd test && make test    # Run all tests
```

- Test inputs: `test/input/*.get` — each file is a raw HTTP GET request line
- Expected outputs: `test/output/*.get` — expected JSON response for each input
- The test harness compares actual output against expected output files
- In CI, a local PostgreSQL geonames database is created from scratch; locally, tests connect to a pre-existing geonames database (configured via `test/cnf/geonames.conf.in`)
- To update expected results after intentional changes, replace files in `test/output/` with the new actual output

## Architecture

Three source files in `autocomplete/`:

- **Plugin.cpp/h** — SmartMet plugin interface (`SmartMetPlugin` subclass). Exports `create`/`destroy` C functions for dynamic loading. Delegates all work to `Autocomplete`. Declares all queries as "fast" (no long processing).
- **Autocomplete.cpp/h** — Core logic. On init, connects to `geonames` and `querydata` engines, reads libconfig configuration, waits for geonames suggest index to be ready, then registers the `/autocomplete` content handler. The `complete()` method parses query parameters and calls `itsGeoEngine->suggest()` (or `suggestDuplicates()`), optionally appending weather forecast values via the querydata engine.
- **ProductParameters.cpp/h** — Maps product names to lists of `Spine::Parameter` for optional forecast enrichment (e.g., product "rich" → Temperature + WeatherSymbol3).

## Key runtime dependencies

The plugin requires two SmartMet engines at runtime (resolved by the server's dynamic loader, not linked directly):

- **Engine::Geonames::Engine** — provides `suggest()` for location autocomplete from the fminames PostgreSQL database
- **Engine::Querydata::Engine** — provides weather forecast data to optionally enrich results

## Configuration

Plugin config (`test/cnf/autocomplete.conf`) uses libconfig format with these settings: `language`, `locale`, `pretty`, `timeformat`, `maxresults`, and a `products` group mapping product names to parameter arrays.

## Query parameters

Key HTTP query parameters: `keyword` (location set), `pattern` (search text), `lang` (language, supports comma-separated list for multi-language), `max` (results per page), `page`, `product` (add forecast data), `feature`/`nofeature` (filter by GeoNames feature code), `debug` (include priority scores), `duplicates`, `pretty`.
