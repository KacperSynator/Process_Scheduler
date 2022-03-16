#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

/* Program description:
 * Program is simulating a process scheduler. Program executes with three arguments (arguments description below).
 * Program works in step mode i.e. firstly it reads input from stdin if there is any, secondly it executes a given
 * schedule method, lastly it prints result of scheduling to stdout. (linefeeder.cpp and repeater.cpp not used).
 * Program ends if there is no input remaining and all CPUS are in sleep state.
 *
 * Input data format:
 * t id prio exec_t
 * t -> time
 * id -> process id
 * prio -> process priority
 * exec_t -> process execution time
 * Note: multiple processes might be given in one line. Last input line must be an "enter" ('\n')
 *
 * Output data format:
 * t cpu1_state cpu2_state ...
 * t -> time
 * cpu_state -> id of executing process or -1 (sleeping)
 * Note: number of columns is equal to the number of CPUS
 *
 * Arguments:
 * arg1 -> schedule method (necessary)
 * arg2 -> number of CPUS (optional, default 1)
 * arg3 -> round robin slice time (optional, default 1)
 *
 * Implemented schedule methods (arg1):
 * 0 -> First Come First Serve (FCFS)
 * 1 -> Shortest Job First (SJF)
 * 2 -> Shortest Remaining Time First (SRTF)
 * 3 -> Round Robin (RR)
 * 4 -> Priority with preemption same priorities scheduled using FCFS (lower the priority number higher the executing priority)
 * 5 -> Priority with preemption same priorities scheduled using SRTF                       --"--
 * 6 -> Priority without preemption same priorities scheduled using FCFS                    --"--
 */


/*! structure that contains all necessary process data */
struct proc_data{
    int id; /*!< process id */
    int priority; /*!< process priority */
    unsigned int exec_time; /*!< process execution time */
    unsigned int remaining_time; /*!< process remaining execution time */

    /*! static function for comparing proc_data structures in term of execution time (same values not swapped) */
    static bool compare_exec_time(proc_data pd1, proc_data pd2) {return (pd1.exec_time < pd2.exec_time);}
    /*! static function for comparing proc_data structures in term of priority (same values not swapped) */
    static bool compare_priority(proc_data pd1, proc_data pd2) {return (pd1.priority < pd2.priority);}
    /*! static function for comparing proc_data structures in term of remaining execution time (same values not swapped) */
    static bool compare_remaining_time(proc_data pd1, proc_data pd2) {return (pd1.remaining_time < pd2.remaining_time);}
};
/*! input operator >> overload for proc_data structure */
std::istream& operator >> (std::istream& is, proc_data& pd)
{
    is >> pd.id >> pd.priority >> pd.exec_time ;
    pd.remaining_time = pd.exec_time;
    return is;
}


/*! puts scheduled processes on CPU/CPUS */
void update_cpus_state(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state)
{
    auto it_proc = proc_list.begin();
    for(auto & cpu_state:cpus_state)
    {
        if(it_proc == proc_list.end())
        {
            cpu_state = -1;
        }
        else
        {
            cpu_state = it_proc->id;
            ++it_proc;
        }
    }
    /* sort processes on CPUS from lower to higher, negative numbers pushed at the end (-1 -> CPU sleep) */
    std::stable_sort(cpus_state.begin(), cpus_state.end(), [](int i, int j){return (i >= 0 && j >= 0) ? i < j : i > j;});
}

/*! First Come First Serve scheduling algorithm */
void fcfs(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state)
{
    /* processes are already sorted */
    update_cpus_state(proc_list, cpus_state);
}

/*! Shortest Job First scheduling algorithm */
void sjf(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state)
{
    /* prevent preemption by moving iterator after executing processes */
    auto proc_it = proc_list.begin();
    for(auto cpu_state: cpus_state)
    {
        if(proc_it == proc_list.end() || cpu_state == -1) break;
        for(auto proc: proc_list)
            if(cpu_state == proc.id) proc_it++;
    }
    /* sort remaining processes in terms of execution time */
    std::stable_sort (proc_it, proc_list.end(), proc_data::compare_exec_time);
    update_cpus_state(proc_list, cpus_state);
}

/*! Shortest Remaining Time First scheduling algorithm */
void srtf(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state)
{
    /* sort processes in terms of execution time */
    std::stable_sort (proc_list.begin(), proc_list.end(), proc_data::compare_remaining_time);
    update_cpus_state(proc_list, cpus_state);
}

/*! Round Robin scheduling algorithm */
void rr(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state, unsigned int rr_time)
{
    for(auto cpu_state: cpus_state)
    {
        if(cpu_state == -1) break;
        auto proc_it = proc_list.begin();
        /* move iterator to executing process */
        while(cpu_state != proc_it->id && proc_it != proc_list.end()) proc_it++;
        if (proc_it == proc_list.end()) continue;
        /* check if executing process needs to be pushed to the end of process list (if RR time slice has ended) */
        unsigned int proc_executed_time = proc_it->exec_time - proc_it->remaining_time;
        if( proc_executed_time > 0 && proc_executed_time % rr_time == 0)
        {
            /* push process to the end of process list  */
            proc_data pd = *proc_it;
            proc_list.erase(proc_it);
            proc_list.push_back(pd);
        }
    }
    update_cpus_state(proc_list, cpus_state);
}

/*! Priority with preemption scheduling algorithm, same priorities are scheduled using FCFS algorithm */
void prio_fcfs(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state)
{
    /* sort processes in terms of priority, same priorities are already sorted */
    std::stable_sort (proc_list.begin(), proc_list.end(), proc_data::compare_priority);
    update_cpus_state(proc_list, cpus_state);
}

/*! Priority with preemption scheduling algorithm, same priorities are scheduled using SRTF algorithm */
void prio_srtf(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state)
{
    /* sort processes in terms of remaining execution time (same priority) */
    std::stable_sort (proc_list.begin(), proc_list.end(), proc_data::compare_remaining_time);
    /* sort processes in terms of priority */
    std::stable_sort (proc_list.begin(), proc_list.end(), proc_data::compare_priority);
    update_cpus_state(proc_list, cpus_state);
}

/*! Priority without preemption scheduling algorithm, same priorities are scheduled using FCFS algorithm */
void prio_fcfs_no_preemption(std::vector<proc_data>& proc_list, std::vector<int>& cpus_state)
{
    /* prevent preemption by moving iterator after executing processes */
    auto proc_it = proc_list.begin();
    for(auto cpu_state: cpus_state)
    {
        if(proc_it == proc_list.end() || cpu_state == -1) break;
        for(auto proc: proc_list)
            if(cpu_state == proc.id) proc_it++;
    }
    /* sort processes in terms of priority, same priorities are already sorted */
    std::stable_sort (proc_it, proc_list.end(), proc_data::compare_priority);
    update_cpus_state(proc_list, cpus_state);
}

/*! checks if all CPUS are sleeping (==-1) */
bool all_cpus_sleeping(std::vector<int>& cpus_state)
{
    if(std::any_of(cpus_state.begin(), cpus_state.end(), [](int i){return i != -1;}))
        return false;
    return true;
}

int main(int argc, char* argv[])
{
    unsigned int method; // scheduling method (arg1)
    unsigned int cpu_count = 1; // number of CPUS (arg2), default 1
    unsigned int rr_time = 1; // Round Robin slice time (arg3), default 1
    // read first argument (schedule method)
    if(argc < 2) throw std::invalid_argument("arg1 not given (schedule method)");
    else method = static_cast<unsigned int>(std::strtol(argv[1], nullptr, 0));
    // check for second argument (CPU count)
    if(argc >= 3) cpu_count = static_cast<unsigned int>(std::strtol(argv[2], nullptr, 0));
    // check for third argument (round robin time step)
    if(argc >= 4) rr_time = static_cast<unsigned int>(std::strtol(argv[3], nullptr, 0));

    std::vector<proc_data> proc_list; // processes execution list
    std::vector<int> cpus_state(cpu_count); // CPU states list
    unsigned int time = 0; // simulation time
    bool read = true; // flag for reading input
    while(read || !all_cpus_sleeping(cpus_state)) // run until there is no input and all CPUS are sleeping
    {
        // read input
        if(read)
        {
            std::string line;
            std::getline(std::cin, line);
            std::istringstream ss(line);
            if (ss.rdbuf()->in_avail() != 0)
            {
                ss >> time;
                while (ss.rdbuf()->in_avail() / 2 >= 3)
                {
                    proc_data pd{};
                    ss >> pd;
                    proc_list.push_back(pd);
                }
            }
            else read = false;
        }
        // run given method
        switch (method)
        {
            case 0: // FCFS
            {
                fcfs(proc_list, cpus_state);
                break;
            }
            case 1: // SJF
            {
                sjf(proc_list, cpus_state);
                break;
            }
            case 2: // SRTF
            {
                srtf(proc_list, cpus_state);
                break;
            }
            case 3: // RR
            {
                rr(proc_list, cpus_state, rr_time);
                break;
            }
            case 4: // Priority_FCFS
            {
                prio_fcfs(proc_list, cpus_state);
                break;
            }
            case 5: // Priority_SRTF
            {
                prio_srtf(proc_list, cpus_state);
                break;
            }
            case 6: // Priority without preemption (FCFS)
            {
                prio_fcfs_no_preemption(proc_list, cpus_state);
                break;
            }
            default: // Wrong method, raises error
                throw std::invalid_argument("invalid schedule method");

        }
        // update proc_list
        for(auto id: cpus_state)
        {
            auto it_proc = proc_list.begin();
            if(id == -1) continue;
            while(it_proc->id != id) ++it_proc;
            // pop an executed process
            if(--(it_proc->remaining_time) == 0)
                proc_list.erase(it_proc);
        }
        // print output
        std::cout << time++;
        for(auto & cpu_state: cpus_state)
            std::cout << " " << cpu_state;
        std::cout << "\n";
    }
    return 0;
}

