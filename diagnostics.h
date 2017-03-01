#ifndef DIAGNOSTICS

/* =========== What sort of DIAGNOSTICS are wanted? ======== */

/* Exactly one of the following should be defined....           */
/* Only change the next few lines, leave rest of file unchanged */

#define SHOWDIAGNOSTICS
/* #define HIDEDIAGNOSTICS    */
/* #define RUNTIMEDIAGNOSTICS */



/* =========== PRIVATE: Rest of file should not normally be changed ==== */

/* Author:  Peter Dickman
 * Editted: 26-10-95
 * Version: 1.02
 *
 * Header file for DIAGNOSTICS package : only modify contents of above section
 */

/* ============= Prototypes for run-time diagnostics ========= */

int  diagnostics_enabled(void);
void enable_diagnostics(void);
void disable_diagnostics(void);

/* ============= Define DIAGNOSTICS macro ==================== */

#ifdef SHOWDIAGNOSTICS
#ifdef DIAGNOSTICS
#error Diagnostics package internal error - only one mode allowed
#error and package has been broken - header should not be loaded twice.
#endif
#include <stdio.h>
#define DIAGNOSTICS printf
#endif

#ifdef HIDEDIAGNOSTICS
#ifdef DIAGNOSTICS
#error Diagnostics package internal error - only one mode allowed
#endif
/*
 * Normally the parenthesised, comma-separated list following the
 * diagnostics macro is the arguments etc for printf. i.e. the
 * parentheses represent function invocation and the commas are
 * argument separators. However, if diagnostics have been disabled
 * the diagnostics macro is a cast to void. In this case the
 * parentheses are simply indicating precedence and the commas are
 * sequencing operators, each argument in turn is evaluated and the
 * last of them handed back. It's immediately cast to void so there
 * is nothing left and the code simply continues. Beware though, any 
 * side-effects in the argument evaluation will still occur. This is
 * good as it means the program behaviour won't be affected by changes
 * the the diagnostics state.
 */
#define DIAGNOSTICS (void) 
#endif

#ifdef RUNTIMEDIAGNOSTICS
#ifdef DIAGNOSTICS
#error Diagnostics package internal error - only one mode allowed
#endif
/*
 * In the other modes the arguments are always evaluated, unfortunately
 * in the RUNTIME diagnostics state the arguments to the printf are
 * only evaluated if diagnostics are enabled. This may be confusing
 * if the argument evaluation has side-effects.
 */
#include <stdio.h>
#define DIAGNOSTICS if (diagnostics_enabled()) printf 
#endif

/* ============= Check all is well =========================== */

#ifndef DIAGNOSTICS
#error Diagnostics package internal error - no option selected
/* Assume HIDEDIAGNOSTICS if nothing specified */
#define DIAGNOSTICS (void)
#endif

#endif
