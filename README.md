# os-lab2

Operating Systems - Lab2: POSIX Threads and Semaphores

### How to compile and run:

    In the terminal, enter:
    1. make
    2. make run

### Explanation of display

    The display is a separate thread that runs a while loop, waiting on a semaphore to be incremented to output statuses and then decerements it after displaying.
    The display function "clears" the terminal with ANSI escape characters written to std::cout after every pass, so it may appear that nothing is happening when there are no changes to statuses. But, in fact, things are happening (it's not just frozen).

    Below is the format of the output.

    Car # is running. The rider is #
    Rider # is waiting in line...
    Rider # is wandering...


### On complettion

    Once all rides have been exhausted, threads are joined and the program exits with "End of simulation" printed to the screen.
