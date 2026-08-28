<div align="center">

# budget-gate

Priority-aware token bucket for throttling memory / IO reads.

C++20 · Win32 · header + static lib

</div>

High-priority work skips the gate. Everything else waits on a refillable budget so you don’t hammer the backend from background threads.

## Why

When a lot of threads read process or kernel memory at once, the hot path dies. This keeps a per-thread priority stack and a shared token bucket:

| Priority | Behavior |
|----------|----------|
| `Critical` | Never waits |
| `Normal` / `Background` | Takes a token, or blocks until refill |

## Quick use

```cpp
#include <budget_gate.hpp>

budget_gate::default_gate().start_refill();

{
    budget_gate::Scope scope( budget_gate::Priority::Critical );
    // hot path — no throttle
    auto base = ReadMemory<std::uint64_t>( player );
}

{
    budget_gate::Scope scope( budget_gate::Priority::Background );
    budget_gate::acquire_default();
    auto hp = ReadMemory<float>( health );
}
```

Wire `acquire_default()` (or `Gate::acquire()`) at the start of every read you care about.

## Build

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Produces a static lib. Public headers live under `include/`.

## Layout

```text
include/budget_gate.hpp      umbrella
include/budget_gate/
  priority.hpp               stack + Scope RAII
  gate.hpp                   token bucket
src/
  priority.cpp
  gate.cpp                   refill thread + acquire
CMakeLists.txt
```

## Knobs

```cpp
budget_gate::Config cfg;
cfg.capacity = 200;    // tokens after each refill
cfg.initial  = 1000;   // starting tokens
cfg.refill_ms = 10;    // Sleep between refills
budget_gate::Gate gate( cfg );
gate.start_refill();
```
