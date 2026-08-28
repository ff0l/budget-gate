#include "budget_gate/priority.hpp"

namespace budget_gate {
namespace {
    thread_local std::stack< Priority > g_stack;
}

Priority current( ) {
    return g_stack.empty( ) ? Priority::Critical : g_stack.top( );
}

void push( Priority level ) {
    g_stack.push( level );
}

void pop( ) {
    if ( !g_stack.empty( ) )
        g_stack.pop( );
}

Scope::Scope( Priority level ) {
    push( level );
}

Scope::~Scope( ) {
    if ( armed_ )
        pop( );
}
} // namespace budget_gate
