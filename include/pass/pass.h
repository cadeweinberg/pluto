// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLUTO_PASS_PASS_H
#define PLUTO_PASS_PASS_H

#include <stdint.h>

struct Context;
struct Module;
struct Function;
struct PassManager;

typedef void *(*context_pass_construct_fn)(struct Context *,
                                           struct PassManager *);
typedef void (*context_pass_destruct_fn)(void *);
typedef int32_t (*context_pass_run)(void *);
typedef void *(*module_pass_construct_fn)(struct Module *,
                                          struct PassManager *);
typedef void (*module_pass_destruct_fn)(void *);
typedef int32_t (*module_pass_run)(void *);
typedef void *(*function_pass_construct_fn)(struct Function *,
                                            struct PassManager *);
typedef void (*function_pass_destruct_fn)(void *);
typedef int32_t (*function_pass_run)(void *);

// the pattern is that the pass manager will call the construct function for a
// pass, and then call the run function for that pass on each item in the module
// (for a module pass) or each function in the module (for a function pass). the
// pass can use the context to get information about the module and the pass
// manager. the pass can also use the PassManager to communicate information to
// other passes, for example, if a pass invalidates the dominator graph, it can
// set a flag in the PassManager to indicate that the dominator graph needs to
// be rebuilt. the pass manager can then check this flag and rebuild the
// dominator graph if necessary.

// overall, tha library form will just present a C API for constructing and
// running passes, and charon will provide a C compiler written using the
// library.

#endif // !PLUTO_PASS_PASS_H
