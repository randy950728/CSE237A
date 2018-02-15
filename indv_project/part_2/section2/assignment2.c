// UCSD CSE237A - WI18
// Important! You WILL NEED TO IMPLEMENT AND SUBMIT this file.
// For more details, please see the instructions in the class website.
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "section1.h"
#include "section2.h"

// You can access the workloads with the task graph
// using the following header file
#include "workload.h"

// You can characterize the workload using the provided funtions of
// these header files in the same way to the part 1
#include "workload_util.h"
#include "cpufreq.h"
#include "pmu_reader.h"

// FREQ_CTL_MIN, FREQ_CTL_MAX is defined here
#include "scheduler.h"

// Functions related to Sample code 3.
static void report_measurement(int, PerfData*);
static void* sample3_init(void*);
static void* sample3_body(void*);
static void* sample3_exit(void*);


void sort_task_length(int* sorted_task, int* task_length, int num_task, int num_start)
{
    int i=0;
    int count    = 0;
    int cur_ptr  = 0;
    int next_ptr = 0;
    bool swapped = false;

    // First create a array of available task
    for(i=0 ; i<num_task ; i++)
    {
        if(task_length[i]!=-1)
        {
            sorted_task[count]=i;
            count++;
        }
    }

    // Bubble sort
    while(1)
    {
        swapped = false;
        for(i=0 ; i<(num_start-1) ; i++)
        {
            cur_ptr  = sorted_task[i];
            next_ptr = sorted_task[i+1];
            if(task_length[cur_ptr]<task_length[next_ptr])
            {
                sorted_task[i]   = next_ptr;
                sorted_task[i+1] = cur_ptr;
                swapped = true;
            }
        }

        if(swapped==false)
            break;
    }
    return;
}

// You can characterize the given workloads with the task graph
// in this function to make your scheduling strategies.
// This is called the start part of the program before the scheduling.
// You need to learn the characteristics within 300 seconds.
// (See main_section2.c)
void learn_workloads(SharedVariable* sv)
{
	// TODO: Fill the body
	// This function is executed before the scheduling simulation.
	// You need to characterize the workloads (e.g., the execution time and
    // memory access patterns) with the task graphs

	// Tip 1. You can get the current time here like:
	// long long curTime = get_current_time_us();
    // Tip 2. You can also use your kernel module to read PMU events,
    // and provided workload profiling code in the same way to the part 1.

    //Time all tasks running time
    //----------------------------------------------------//
    set_by_max_freq();
    int w_idx;
    int num_workloads = get_num_workloads();
    double LLC_miss_rate = 0.0;
    double L1_miss_rate = 0.0;
    sv-> path_len       = (int*) malloc(num_workloads*sizeof(int));
    sv-> final_schedule = (int*) malloc(num_workloads*sizeof(int));
    sv-> schedule_prog  = (int*) malloc(num_workloads*sizeof(int));
    sv-> max_freq       = (int*) malloc(num_workloads*sizeof(int));

    sv-> freed = 0;
    sv-> num_workloads  = num_workloads;
    // TimeType* exe_time  = (TimeType*) malloc(num_workloads*sizeof(TimeType));

    for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    {
        //Aquire the workload and initilize it
        const WorkloadItem* workload_item = get_workload(w_idx);
        // void* init_ret = workload_item->workload_init(NULL);

        //Timing the execution time of each task
        // printf("Workload body %2d starts.\n", w_idx);
        // exe_time[w_idx] = get_current_time_us();
        // void* body_ret = workload_item->workload_body(init_ret);
        // exe_time[w_idx] = get_current_time_us()-exe_time[w_idx];
        // printf("Workload body %2d finishes.\n", w_idx);

        // void* exit_ret = workload_item->workload_exit(init_ret);

        register_workload(0, workload_item->workload_init, workload_item->workload_body, workload_item->workload_exit);
        PerfData perf_msmts[MAX_CPU_IN_RPI3];
        run_workloads(perf_msmts);
        unregister_workload_all();

        LLC_miss_rate = (double)perf_msmts->llcmiss/(double)perf_msmts->llcaccess;
        L1_miss_rate  =(double)perf_msmts->l1miss/(double)perf_msmts->l1access;
        printf("workload-%d, LLC Miss rate: %5f  L1 Miss rate: %5f ",w_idx,LLC_miss_rate*100,L1_miss_rate*100);
        if(LLC_miss_rate >= 0.15 || L1_miss_rate>=0.25)
        {
            printf("Operation mode: LOW\n");
            sv-> max_freq[w_idx]=false;
        }

        else
        {
            printf("Operation mode: LOW\n");
            sv-> max_freq[w_idx]=true;
        }

    }


    // Figure out task path
    //----------------------------------------------------//
    int i=0;
    int cur_ptr=0;
    int offset = 0;
    int num_start_tasks=0;
    int successor_idx = 0;
    int* task_visted  = (int*) malloc(num_workloads*sizeof(int));
    int* task_length  = (int*) malloc(num_workloads*sizeof(int));
    bool* is_starting_tasks = (bool*)malloc(num_workloads * sizeof(bool));

    //Initialize all tasks as a starting task
    for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    {
        is_starting_tasks[w_idx] = true;
        sv->final_schedule[w_idx] = -1;
        sv->schedule_prog[w_idx] = true;
        sv->path_len[w_idx] = 0;
    }

    //Set all tasks that are successors to false
    for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    {
        successor_idx = get_workload(w_idx)->successor_idx;
        if (successor_idx == NULL_TASK)
            continue;
        is_starting_tasks[successor_idx] = false;
    }

    //Calculate number of starting task, initialize task_visited
    for(i=0 ; i<num_workloads ; ++i )
    {
        task_visted[i]=0;
        if (!is_starting_tasks[i])
            continue;
        else
            num_start_tasks+=1;
    }

    //Calculate task legnth of each starting task
    for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    {
        successor_idx = get_workload(w_idx)->successor_idx;
        while (successor_idx != NULL_TASK)
        {
            successor_idx = get_workload(successor_idx)->successor_idx;
            sv->path_len[w_idx]+=1;
        }
    }

    //Calculate task legnth of each starting task, calculate dependencies
    for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    {
        successor_idx = get_workload(w_idx)->successor_idx;

        if(successor_idx != NULL_TASK)
            task_visted[successor_idx]+=1;

        if (!is_starting_tasks[w_idx])
        {
            task_length[w_idx]=-1;
        }

        else
        {
            task_length[w_idx]=1;
            task_visted[w_idx]=0;

            printf("%2d", w_idx);

            while (successor_idx != NULL_TASK)
            {
                printf(" -> %2d", successor_idx);
                successor_idx = get_workload(successor_idx)->successor_idx;
                task_length[w_idx]+=1;
            }
            printf("\n");
        }
    }

    printf("dependency array: ");
    for(i=0 ; i<num_workloads ; i++)
    {
        printf(" %d ",task_visted[i]);
    }
    printf("\n");
    int min=0;

    // Calculate total number of starting tasks
    while(sv->final_schedule[num_workloads-1]==-1)
    {
        printf("number of task: %d,  number of start task: %d\n",num_workloads,num_start_tasks);

        // Look for task with longest path
        int* sort_task = (int*) malloc(num_start_tasks*sizeof(int));
        sort_task_length(sort_task, task_length, num_workloads, num_start_tasks);

        //Move the current task into schedule list
        min =2;
        if(num_start_tasks<2)
            min=num_start_tasks;

        for(i=0 ; i<min ; i++)
        {
            sv->final_schedule[i+offset] = sort_task[i];
        }
        offset+= min;

        //Update current dependency
        for(i=0 ; i<min ; i++)
        {
            cur_ptr = sort_task[i];
            int successor_idx = get_workload(cur_ptr)->successor_idx;
            if (successor_idx == NULL_TASK)
            {
                task_length[cur_ptr]=-1;
                continue;
            }

            task_visted[successor_idx]-=1;
            if(task_visted[successor_idx]==0)
            {
                task_visted[successor_idx] = 1;
                task_length[successor_idx] = task_length[cur_ptr]-1;
                task_length[cur_ptr] = -1;
            }

            else
                task_length[cur_ptr] = -1;
        }

        num_start_tasks=0;
        for(i=0 ; i<num_workloads ; ++i )
        {
            if (task_length[i]==-1)
                continue;
            else
                num_start_tasks+=1;
        }
        free(sort_task);
    }

    printf("Workload sequence: %d",sv->final_schedule[0]);
    for(i=1 ; i<num_workloads ; i++)
    {
        printf(" -> %d",sv->final_schedule[i]);
    }
    printf("\n");
    free(task_length);
    free(task_visted);
    free(is_starting_tasks);
}

// Select a workload index among schedulable workloads.
// This is called by the provided scheduler base (schedule() function.)
// You have to return a TaskSelection structure specifying
// the selected task index and the frequency (FREQ_CTL_MIN or FREQ_CTL_MAX)
// that you want to execute.
TaskSelection select_workload(
        SharedVariable* sv, const int core,
        const int num_workloads,
        const bool* schedulable_workloads, const bool* finished_workloads) {
	// TODO: Fill the body
    // This function is executed inside of the provided scheduler code.
    // You need to implement an energy-efficient LIST scheduler.

    //////////////////////////////////////////////////////////////
    // Sample scheduler: A naive scheduler that satisfies the task dependency
    // This scheduler selects a possible task in order of the task index,
    // and always uses the minumum frequency.
    // This doesn't guarantee any of task deadlines.
    //////////////////////////////////////////////////////////////
    TaskSelection task_selection;
    int i;
    int max_idx=0;
    int max_path_len=-10;
    int selected_worload_idx;


    for(i = 0; i < num_workloads; i++)
    {
        if(schedulable_workloads[i]==true && sv->path_len[i] > max_path_len)
        {
            max_path_len= sv->path_len[i];
            max_idx=i;
        }
    }
    task_selection.task_idx=max_idx;
    // for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    // {
    //     // Choose one possible task
    //     if (schedulable_workloads[w_idx])
    //     {
    //         task_selection.task_idx = w_idx;
    //         break;
    //     }
    // }

    // Choose the minimum frequency
    if(sv-> max_freq[max_idx]==true)
        task_selection.freq = FREQ_CTL_MAX; // You can change this to FREQ_CTL_MAX FREQ_CTL_MIN
    else
        task_selection.freq = FREQ_CTL_MIN;
    return task_selection;
    //////////////////////////////////////////////////////////////
}

// This function is called before scheduling 16 tasks.
// You may implement some code to evaluate performance and power consumption.
// (This is called in main_section2.c)
void start_scheduling(SharedVariable* sv) {
    sv->exe_time = get_current_time_us();
	// TODO: Fill the body if needed
}

// This function is called whenever all workloads in the task graph
// are finished. You may evaluate performance and power consumption
// of your implementation here.
// (This is called in main_section2.c)
void finish_scheduling(SharedVariable* sv)
{
    float time = (float) (get_current_time_us()- sv->exe_time);
    // time = time/1000000.0;
    printf("Total exe time: %d\n", get_current_time_us()- sv->exe_time);
    if(sv->freed==0)
    {
        sv->freed=1;
        free(sv->schedule_prog);
        free(sv->final_schedule);
        free(sv->path_len);
        free(sv->max_freq);
    }
}

/////////////////////////////////////////////////////////////////////////////
// From here, We provide functions related to Sample 3.
/////////////////////////////////////////////////////////////////////////////

// This function is same to one provided in part 1.
static void report_measurement(int freq, PerfData* perf_msmts) {
    int core;
    for (core = 0; core < MAX_CPU_IN_RPI3; ++core) {
        PerfData* pf = &perf_msmts[core];
        if (pf->is_used == 0)
            continue;

        TimeType time_estimated = (TimeType)pf->cc/(TimeType)(freq/1000);
        printf("[Core %d] Execution Time (us): %lld\n", core, time_estimated);

        printf("[Core %d] Cycle Count: %u\n", core, pf->cc);
        printf("[Core %d] Instructions: %u\n", core, pf->insts);

        printf("[Core %d] L1 Cache Accesses: %u\n", core, pf->l1access);
        printf("[Core %d] L1 Cache Misses: %u\n", core, pf->l1miss);
        if (pf->l1access != 0)
            printf("[Core %d] L1 Miss Ratio: %lf\n",
                    core, (double)pf->l1miss/(double)pf->l1access);

        printf("[Core %d] LLC Accesses: %u\n", core, pf->llcaccess);
        printf("[Core %d] LLC Misses: %u\n", core, pf->llcmiss);
        if (pf->llcaccess != 0)
            printf("[Core %d] LLC Miss Ratio: %lf\n",
                    core, (double)pf->llcmiss/(double)pf->llcaccess);

        printf("[Core %d] iTLB Misses: %u\n", core, pf->iTLBmiss);
    }
}

// Initialize the first five tasks
static void* p_ret_workload[5];
static void* sample3_init(void* unused) {
    p_ret_workload[0] = get_workload(0)->workload_init(NULL);
    p_ret_workload[1] = get_workload(1)->workload_init(NULL);
    p_ret_workload[2] = get_workload(2)->workload_init(NULL);
    p_ret_workload[3] = get_workload(3)->workload_init(NULL);
    p_ret_workload[4] = get_workload(4)->workload_init(NULL);
}

// Execute the body of the first three tasks sequentially
static void* sample3_body(void* unused) {
    get_workload(0)->workload_body(p_ret_workload[0]);
    get_workload(1)->workload_body(p_ret_workload[1]);
    get_workload(2)->workload_body(p_ret_workload[2]);
    get_workload(3)->workload_body(p_ret_workload[3]);
    get_workload(4)->workload_body(p_ret_workload[4]);
}

// Execute the exit function of the first five tasks sequentially
static void* sample3_exit(void* unused) {
    get_workload(0)->workload_exit(p_ret_workload[0]);
    get_workload(1)->workload_exit(p_ret_workload[1]);
    get_workload(2)->workload_exit(p_ret_workload[2]);
    get_workload(3)->workload_exit(p_ret_workload[3]);
    get_workload(4)->workload_exit(p_ret_workload[4]);
}
