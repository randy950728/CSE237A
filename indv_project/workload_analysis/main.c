// UCSD CSE237A - WI18
// Important! You need to modify this file, but WILL NOT SUBMIT this file.
// You can characterize the given workloads by chainging this file,
// and WILL SUBMIT the analysis report based on the characterization results.
// For more details, please see the instructions in the class website.

// main.c: Main file to characterize different workloads

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpufreq.h"
#include "workload_util.h"
#include "workload.h"

// The function can be called after finishing workload(s)
void report_measurement(int freq, PerfData* perf_msmts) {
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

int main(int argc, char *argv[]) {
    // 0. Initialize the workload
    TimeType start_time;
    PerfData perf_msmts[MAX_CPU_IN_RPI3];
    int freq = 0;
                                    //Test-1//
    //-------------------------------------------------------------------------------//
    printf("************************************\n");
    printf("Initialization-test1.\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    //register_workload(1, workload2_init, workload2_body, workload2_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");

                                    //Test-2//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test2.\n");
    register_workload(0, workload2_init, workload2_body, workload2_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");


                                    //Test-3//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test3.\n");
    register_workload(0, workload3_init, workload3_body, workload3_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");

                                        //Test-4//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test4\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    register_workload(1, workload2_init, workload2_body, workload2_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");

                                        //Test-5//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test5\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    register_workload(1, workload3_init, workload3_body, workload3_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");

                                        //Test-6//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test6\n");
    register_workload(0, workload2_init, workload2_body, workload2_exit);
    register_workload(1, workload3_init, workload3_body, workload3_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");

                                        //Test-7//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test7\n");
    register_workload(0, workload1_init, workload1_body, workload1_exit);
    register_workload(1, workload1_init, workload1_body, workload1_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");

                                        //Test-8//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test8\n");
    register_workload(0, workload2_init, workload2_body, workload2_exit);
    register_workload(1, workload2_init, workload2_body, workload2_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n\n");

                                        //Test-9//
    //-------------------------------------------------------------------------------//
    // 0. Initialize the workload
    printf("************************************\n");
    printf("Initialization-test9\n");
    register_workload(0, workload3_init, workload3_body, workload3_exit);
    register_workload(1, workload3_init, workload3_body, workload3_exit);

    // 1. Set CPU frequency
    // ****** YOU MAY NEED TO CHANGE THIS TO USE set_by_min_freq() ******
    set_userspace_governor();
    set_by_min_freq();
    freq = get_cur_freq();

    // 2. Run workload
    printf("Characterization starts.\n");
    start_time = get_current_time_us();
    run_workloads(perf_msmts);

    // 3. Here, we get elapsed time and performance counters.
    printf("Total Execution time (us): %lld at %d\n",
            get_current_time_us() - start_time, get_cur_freq());
    report_measurement(freq, perf_msmts);

    // 4. Finish the program
    unregister_workload_all();
    set_ondemand_governor();
    printf("************************************\n");

    return 0;
}
