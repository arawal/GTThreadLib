-This package contains source files for a pre-emptive round-robin scheduler.
-The code has been developed with Ubuntu 14.04 running as a VM over VMWare on Windows 8.1 64 bit processor.
-The pre-emptive scheduler has been implemented as follows
---Every thread is intialized with a time interval.
---An alarm is set for that time once the thread starts executing.
---On alarm, the context is switched to the next thread in the queue.
---The queue has been implemented as nodes of gtthread strtuct type.
---The queue is referenced using 3 pointers, viz. tq_head, tq_tail and running.
---Instead of cycling the pointers in the queue for the round-robin scheduler, the "running" pointer is shifted around the queue.
---The status of any thread is maintained with the status variable.
---The status can take either of the 5 values mentioned below.
     -1 for a cancelled thread
      0 for a exited thread
      1 for a completed thread
      2 for a running thread
      9 as a default for a recently initialized thread
---The run_thread and schedule_thread are responsible for executing the threads and maintaing the queue.

-To compile the library, just navigate to the project folder and run "make" from the terminal.
-To run any test using the library, execute the command "gcc test.c gtthread.a". The output can be seen by executing "./a.out".
-The dining philosophers problem is compiled with the make command. To see the output, execute "./dining_philosopher".

-The dining philosopher problem has a number of solutions, with some variations to the problem.
-In the current code, the deadlock is avoided by ensuring the order in which the philosophers pick up their chopsticks.
-In the implemented approach, this is done by having the even numbered (2 and 4 of the 5) pick up the left chopsticks before the right.
-All others pick up the right chopsticks before the left ones.
-On the other hand, while releasing, its the opposite.
-This ensures that two adjacent philosophers will either have both or none for the most part.

-This project was really a "from the scratch" for me. I would like to point out that I've had to take a lot of help from various sources to complete it.
-I'm not sure if I have violated the Georgia Tech pledge with this (hope not).
-But I feel that now I have a more deeper and better understanding of how threads operate.
-However, I've not been able to test all test cases on my code.
-One thing that DOESNT WORK is if the test cases have #include <gtthread.h> instead of "gtthread.h".
-I've learned a lot, refreshed a lot of my C coding skills with this project.
-In essence it was FUN and invigorating!!!
