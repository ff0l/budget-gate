#pragma once

#include <cstdint>
#include <stack>

namespace budget_gate {

enum class Priority : std::uint8_t {
    Critical = 1, // bypasses the budget
    Normal = 2,
    Background = 3
};

Priority current( );
void push( Priority level );
void pop( );

// RAII guard — push on construct, pop on destroy.
class Scope {
public:
    explicit Scope( Priority level );
    ~Scope( );

    Scope( const Scope& ) = delete;
    Scope& operator=( const Scope& ) = delete;

private:
    bool armed_{ true };
};

} // namespace budget_gate
