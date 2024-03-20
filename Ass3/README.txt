Assignment 3 
CMPT 300 
Spring 2024
Huynh Vuong The Nguyen
301450119

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
