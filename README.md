# Process_Scheduler

Program is simulating a process scheduler. Program executes with three arguments (arguments description below).
Program works in step mode i.e. firstly it reads input from stdin if there is any, secondly it executes a given schedule method,
lastly it prints result of scheduling to stdout. Program ends if there is no input remaining and all CPUS are in sleep state.


## Input data format:
Sample data files can be found in [data/](data/)
```
time proccess_id process_priority process_exec_time
```
**Note:** multiple processes might be given in one line. Last input line must be new line character ('\n').
 
## Output data format:
```
 time cpu1_state cpu2_state ...
```
If CPU is sleeping -1 is set.  
**Note:** number of columns is equal to the number of CPUS

## Implemented schedule methods:
 0 -> First Come First Serve (FCFS)  
 1 -> Shortest Job First (SJF)  
 2 -> Shortest Remaining Time First (SRTF)  
 3 -> Round Robin (RR) 
 4 -> Priority with preemption same priorities scheduled using FCFS  
 5 -> Priority with preemption same priorities scheduled using SRTF                       
 6 -> Priority without preemption same priorities scheduled using FCFS      
**Note:** lower the priority number higher the executing priority  

## Getting started
1. Clone repo
```bash
git clone https://github.com/KacperSynator/Process_Scheduler.git
```

2. Compile
```bash
g++ -o process_scheduler main.cpp
```

3. Run
```bash
./process_scheduler <schedule method> [number of CPUS] [rr slice time] < <data_file>
```
`number of CPUS` default is 1  
`rr slice time` is only used by Round Robin (3) method (default 1).

## Example
```bash
./process_scheduler 0 2 < data/sched1.in
0 1 -1
1 1 2
2 2 -1
3 2 3
4 2 3
5 3 4
6 4 5
7 4 6
8 6 7
9 6 7
10 6 7
11 -1 -1
```
 
