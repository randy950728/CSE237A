// THIS IS THE TA SOLUTION
// Enables 6 perf counters, for WI18 CSE237A

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your name");
MODULE_DESCRIPTION("PMUON");

void set_pmu(void* dummy) {
    unsigned int v;
    printk("Turn PMU on Core %d\n", smp_processor_id());

    // 1. Enable "User Enable Register"
    asm volatile("mcr p15, 0, %0, c9, c14, 0\n\t" :: "r" (0x00000001));

    // 2. Reset i) Performance Monitor Control Register(PMCR),
    // ii) Count Enable Set Register, and
    // iii) Overflow Flag Status Register
    asm volatile ("mcr p15, 0, %0, c9, c12, 0\n\t" :: "r"(0x00000017));
    asm volatile ("mcr p15, 0, %0, c9, c12, 1\n\t" :: "r"(0x8000003f));
    asm volatile ("mcr p15, 0, %0, c9, c12, 3\n\t" :: "r"(0x8000003f));

    // 3. Disable Interrupt Enable Clear Register
    asm volatile("mcr p15, 0, %0, c9, c14, 2\n\t" :: "r" (~0));

    // 4. Read how many event counters exist
    asm volatile("mrc p15, 0, %0, c9, c12, 0\n\t" : "=r" (v)); // Read PMCR
    printk("We have %d configurable event counters on Core %d\n",
            (v >> 11) & 0x1f, smp_processor_id());

    // 5. Set six event counter registers (Project Assignment you need to IMPLEMENT)
    //1st Event counter: # of instructions exectued: 0x08
	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r" (0x00));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r" (0x08));

	//2nd Event counter: # of L1 data cache access: 0x04
	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r" (0x01));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r" (0x04));

	//3rd Event counter: # of L1 data cache miss(refill): 0x03
	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r" (0x02));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r" (0x03));

	//4th Event counter: # of L2 data cache access: 0x16
	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r" (0x03));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r" (0x16));

	//5th Event counter: # of L2 data cache miss: 0x17
	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r" (0x04));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r" (0x17));

	//6th Event counter: # of L1 Instruction TLB miss(refill): 0x02
	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r" (0x05));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r" (0x02));

}


int init_module(void) {
    // Set Performance Monitoring Unit (aka Performance Counter) across all cores
    on_each_cpu(set_pmu, NULL, 1);
    printk("Ready to use PMU\n");
    return 0;
}

void cleanup_module(void) {
    printk("PMU Kernel Module Off\n");
}


