This readme document describes the process for executing banking program files.

Server Information Used: username@csx3.cs.okstate.edu

1) Log into a unix based kernel via CSX servers
* ssh username@csx3.cs.okstate.edu

2) Using WinSCP transfer files to the server in a specific directory.
* launch WinSCP and connect to server

3) Navigate to directory
* cd "dir"

3) Use gcc and file name to compile
* gcc driver.c

4) Use ./a.out "commands.txt" to execute the most recently compiled file and view the result.
* ./a.out sampleInput.txt

Notes about running:
* Your commands.txt file contains all transactions for the banking process.
* while(1) blocks are commented in the ipc to identify deadlock testing locations.
* pfkill -f "a.out" may be used in case testing deadlock creates runaway processes.
* The default timeout timer is 3 seconds.

Compilable File Names:
driver.c

-Logan Larsh, Cole Aldridge, Josh Hernandez