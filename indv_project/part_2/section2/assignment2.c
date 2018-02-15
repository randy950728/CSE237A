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

    int counter=0;
    TimeType time_estimated;
    PerfData perf_msmts[MAX_CPU_IN_RPI3];
    set_by_max_freq();

    for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    {
        //Aquire the workload and initilize it
        const WorkloadItem* workload_item = get_workload(w_idx);
        register_workload(0, workload_item->workload_init, workload_item->workload_body, workload_item->workload_exit);

        run_workloads(perf_msmts);
        unregister_workload_all();

        LLC_miss_rate = 100.0*(double)perf_msmts->llcmiss/(double)perf_msmts->llcaccess;
        L1_miss_rate  = 100.0*(double)perf_msmts->l1miss/(double)perf_msmts->l1access;
        time_estimated = (TimeType)perf_msmts->cc/(TimeType)(get_cur_freq()/1000);
        printf("workload-%d, LLC Miss rate: %5f  L1 Miss rate: %5f Execution Time (us): %lld ",w_idx,LLC_miss_rate,L1_miss_rate,time_estimated);
        if(time_estimated<50000 && counter<=4)
        {
            printf("Operation mode: LOW\n");
            sv-> max_freq[w_idx]=false;
            counter++;
        }

        else
        {
            printf("Operation mode: HIGH\n");
            sv-> max_freq[w_idx]=true;
        }

    }

    TimeType total_time=0;
    int all_high=false;
    //verify run time
    //----------------------------------------------------//
    do
    {
        for (w_idx = 0; w_idx < num_workloads; ++w_idx)
        {
            if(sv->max_freq[w_idx]== true)
                set_by_max_freq();
            else
                set_by_min_freq();

            //Aquire the workload and initilize it
            const WorkloadItem* workload_item = get_workload(w_idx);
            register_workload(0, workload_item->workload_init, workload_item->workload_body, workload_item->workload_exit);
            run_workloads(perf_msmts);
            unregister_workload_all();

            time_estimated = (TimeType)perf_msmts->cc/(TimeType)(get_cur_freq()/1000);

            printf("Execution Time (us): %lld \n",time_estimated);
            total_time+=time_estimated;
        }
        printf("total exe time: %lld, worst: %lf\n",total_time, ((double)time_estimated*1.15/2.0));

        if( ((double)total_time*1.15/2.0) > 1000000)
        {
          for (w_idx = 0; w_idx < num_workloads; ++w_idx)
          {
            if(sv->max_freq[w_idx]==true)
            {
                sv->max_freq[w_idx]= false;
                all_high = false;
                break;
            }

            all_high=true;
          }

        }
    }while( ((double)total_time*1.15/2.0)> 1000000 && all_high==false);
    int i=0;
    int cur_ptr=0;
    int offset = 0;
    int num_start_tasks=0;
    int successor_idx = 0;
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
        if (!is_starting_tasks[w_idx])
                continue;
        successor_idx = get_workload(w_idx)->successor_idx;

        printf("%2d", w_idx);
        while (successor_idx != NULL_TASK)
        {
            printf(" -> %2d", successor_idx);
            successor_idx = get_workload(successor_idx)->successor_idx;
        }
        printf("\n");

    }

    printf("path-len array: ");
    for(i=0 ; i<num_workloads ; i++)
    {
        printf("%d-%d  ",i,sv->path_len[i]);
    }
    printf("\n");

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
    int w_idx;
    // for (w_idx = 0; w_idx < num_workloads; ++w_idx)
    // {
    //     // Choose one possible task
    //     if (schedulable_workloads[w_idx])
    //     {
    //         task_selection.task_idx = w_idx;
    //         break;
    //     }
    // }



    for(i = 0; i < num_workloads; i++)
    {
        if(schedulable_workloads[i]==true && sv->path_len[i] > max_path_len)
        {
            max_path_len= sv->path_len[i];
            max_idx=i;
        }
    }
    task_selection.task_idx=max_idx;
    // printf("core-%d running task-%d\n",core,max_idx);

    // printf("available workload: %d-%d",0,schedulable_workloads[0]);
    // for (w_idx = 1; w_idx < num_workloads; ++w_idx)
    // {
    //     // Choose one possible task
    //     printf("  %d-%d",w_idx,schedulable_workloads[w_idx]);
    // }
    // printf("\n");

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
    printf("Total exe time: %llu\n", get_current_time_us()- sv->exe_time);
    if(sv->freed==0)
    {
        sv->freed=1;
        free(sv->schedule_prog);
        free(sv->final_schedule);
        // free(sv->path_len);
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
