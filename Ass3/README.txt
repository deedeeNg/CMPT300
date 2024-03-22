Assignment 3 
CMPT 300 
Spring 2024
Huynh Vuong The Nguyen
301450119

Here is the input structure for my OS simulations

Create: C (priority)
Fork: F 
Kill: K (pid)
Exit: E
Quantum: Q


Some comments about how my implementation works

1. 
If there are no processes in the system, 
then kill init process or exit will terminate 
the simulations

2.
When the process finish running at quantum
time. The priority will be decreased by 1
or the same if they are already in lowest
level

3. 
If there are no processes waiting in 
wait queue. The init process will run

4. 
The init process can't send, reply, receive
nor getting blocked by the user

5.
A process only receive one message from
the first process that sent it. If there
are multiple messages send to the process,
It will receive only first one and send 
error if multiple message coming 

6. 
The process only reply to another process
if they are blocking in sending operation
(Or we can say they are waiting reply)
If they reply to the process that not
waiting for the reply then send error

7.
There are some exceptional behaviour
while you typing input and it read wrong
because the OS behaviour. So if you getting
that, please compile it again!!!

8. 
If process receive a message, then system
will print the message when it's running 

9.
Fork function will not copy the message
from the original process

10.
Init process cannot be blocked by send,
receive nor semaphores

11.
When the process has been blocked by send
and receive operations, when it going back
to ready queue, it means there is message 
send or reply to them so that they can be 
unblocked. when their turn to running, the 
message will automatically show in the screen.