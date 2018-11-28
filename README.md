# Producer-Consumer in C 
# the producer consumer problem between two distincts process, sharing a queue. 

# This "project" has a intention of show how two process run sharing the same queue
# that use the same block of physical memory, but occupy differents addresses of virtual memory.

# The two process only can use one at a time, to do this we need to share a set of semaphore.
# The empty, full and mutex semaphore controls the queue. 

# This only works on linux, it's using system calls to do everything or almost everything. 
# For precaution start with the consumer. If something goes wrong, run "./clean", to release the 
# shared memory and the array of semaphore.    
# That's it. If you have some question, look at the code.  
