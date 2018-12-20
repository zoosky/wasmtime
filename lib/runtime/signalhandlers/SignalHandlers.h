#ifndef signal_handlers_h
#define signal_handlers_h

#include <stdint.h>
#include <setjmp.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Record the Trap code and wasm bytecode offset in TLS somewhere
void RecordTrap(const uint8_t* pc);

// Initiate an unwind.
void Unwind(void);

// Trap initialization state.
struct TrapContext {
    bool triedToInstallSignalHandlers;
    bool haveSignalHandlers;
};

// This function performs the low-overhead signal handler initialization that we
// want to do eagerly to ensure a more-deterministic global process state. This
// is especially relevant for signal handlers since handler ordering depends on
// installation order: the wasm signal handler must run *before* the other crash
// handlers and since POSIX signal handlers work LIFO, this function needs to be
// called at the end of the startup process, after other handlers have been
// installed. This function can thus be called multiple times, having no effect
// after the first call.
bool
EnsureEagerSignalHandlers(void);

// Assuming EnsureEagerProcessSignalHandlers() has already been called,
// this function performs the full installation of signal handlers which must
// be performed per-thread. This operation may incur some overhead and
// so should be done only when needed to use wasm.
bool
EnsureDarwinMachPorts(void);

/// Call the given callee, passing the given argument values, and catch any traps
/// it raises, returning true if no trap occurred.
bool CallTrampoline(const void *callee, uint8_t *values_vec, void *vmctx);

/// Call the given callee, passing it the vmctx argument, and catch any traps it
/// raises, returning true if no trap occurred.
bool Call(const void *callee, void *vmctx);

/// Record the last known usable stack pointer value.
void SetLastKnownUsableSP(void *p);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // signal_handlers_h
