#pragma once

#include "priority.hpp"

#include <cstddef>

namespace budget_gate {

struct Config {
    int capacity = 200;          // tokens after each refill
    int initial = 1000;          // tokens at start
    unsigned refill_ms = 10;     // Sleep between refills
};

// Shared token bucket. Critical priority never waits.
class Gate {
public:
    explicit Gate( Config config = {} );
    ~Gate( );

    Gate( const Gate& ) = delete;
    Gate& operator=( const Gate& ) = delete;

    void start_refill( );
    void stop_refill( );

    // Block until a token is available (no-op for Critical).
    void acquire( );

    int remaining( ) const;
    const Config& config( ) const { return config_; }

private:
    Config config_;
    struct State;
    State* state_;
};

// Default process-wide gate used by acquire_default().
Gate& default_gate( );
void acquire_default( );

} // namespace budget_gate
