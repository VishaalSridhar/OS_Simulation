VISHAAL SRIDHAR

# CPU Scheduling Simulator

## Building and running the program

To build the main program (called 'simulator' by default), type:

`make`

To both build and run the program with the example simulation file and no
command line flags, you can use the following shortcut:

`make run`

If you want to run your program with flags, you can be explicit:

`./simulator --verbose --per_thread example_simulation`

or short-form:

`./simulator -vt example_simulation`

## Quick Reference
src/main.cpp:
    Main file to run the simulation.

src/simulation.h, src/simulation.cpp:
    Simulation functions which contains functionality to simulate OS operations

src/types/:
    Implementation of thread class.

src/algorithms/:
    Implementation of first-come-first-serve, round robin, and priority schedulers.

src/util/:
    Handles the various flags to the program as well as the printing functions.