#include "budget_gate/gate.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace budget_gate {

struct Gate::State {
    mutable std::mutex mutex;
    std::condition_variable cv;
    int tokens = 0;
    bool running = false;
    std::thread worker;
};

Gate::Gate( Config config )
    : config_( config )
    , state_( new State ) {
    state_->tokens = config_.initial;
}

Gate::~Gate( ) {
    stop_refill( );
    delete state_;
    state_ = nullptr;
}

void Gate::start_refill( ) {
    if ( state_->running )
        return;

    state_->running = true;
    state_->worker = std::thread( [ this ] {
        while ( state_->running ) {
            {
                std::lock_guard< std::mutex > lock( state_->mutex );
                state_->tokens = config_.capacity;
            }
            state_->cv.notify_all( );
            Sleep( config_.refill_ms );
        }
    } );
}

void Gate::stop_refill( ) {
    if ( !state_->running )
        return;

    state_->running = false;
    state_->cv.notify_all( );
    if ( state_->worker.joinable( ) )
        state_->worker.join( );
}

void Gate::acquire( ) {
    if ( current( ) == Priority::Critical )
        return;

    std::unique_lock< std::mutex > lock( state_->mutex );
    state_->cv.wait( lock, [ this ] {
        return state_->tokens > 0 || !state_->running;
    } );

    if ( state_->tokens > 0 )
        --state_->tokens;
}

int Gate::remaining( ) const {
    std::lock_guard< std::mutex > lock( state_->mutex );
    return state_->tokens;
}

Gate& default_gate( ) {
    static Gate gate;
    return gate;
}

void acquire_default( ) {
    default_gate( ).acquire( );
}

} // namespace budget_gate
