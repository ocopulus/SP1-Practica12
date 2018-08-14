    #include <linux/fs.h>
    #include <linux/hugetlb.h>
    #include <linux/init.h>
    #include <linux/kernel.h>
    #include <linux/sched.h>
    #include <linux/module.h>
    #include <linux/seq_file.h>
    #include <linux/proc_fs.h>
    #include <linux/cpumask.h>
    #include <linux/interrupt.h>
    #include <linux/kernel_stat.h>
    #include <linux/slab.h>
    #include <linux/time.h>
    #include <linux/irqnr.h>
    #include <linux/cputime.h>
    #include <linux/tick.h>	
    #include <linux/mm.h>
    #include <linux/mman.h>
    #include <linux/mmzone.h>
    #include <linux/quicklist.h>
    #include <linux/swap.h>
    #include <linux/vmstat.h>
    #include <linux/atomic.h>
    #include <linux/smp.h>
    #include <linux/timex.h>
    #include <linux/string.h>
    #include <linux/cpufreq.h>
    #include <linux/delay.h>

	static u64 get_idle_time(int cpu)
	{
		u64 idle = -1ULL;
		u64 idle_usecs = -1ULL;

		if (cpu_online(cpu)) idle_usecs = get_cpu_idle_time_us(cpu, NULL);

		if (idle_usecs == -1ULL)
			idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
		else
			idle = usecs_to_cputime64(idle_usecs);
		return idle;
	}

	static u64 get_iowait_time(int cpu)
	{
		u64 iowait = -1ULL;
		u64 iowait_usecs = -1ULL;

		if (cpu_online(cpu))
			iowait_usecs = get_cpu_iowait_time_us(cpu, NULL);

		if (iowait_usecs == -1ULL)
			iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
		else
			iowait = usecs_to_cputime64(iowait_usecs);

		return iowait;
	}


    static int meminfo_proc_show(struct seq_file *m, void *v)
    {
	struct sysinfo i;
	si_meminfo(&i);
	int x;
	u64 user = 0;
	u64 nice = 0;
	u64 system = 0;
	u64 idle = 0;
	u64 iowait = 0;
	u64 irq = 0;
	u64 softirq = 0;
	u64 steal = 0;
	u64 guest = 0;
	u64 guest_nice = 0;
	u64 sum = 0;
	u64 sum_softirq = 0;
	u64 tiempo = 0;
	u64 espera = 0;
	u64 porctiempo = 0;
        int porcram = 0;

	struct timespec64 boottime;

	getboottime(&boottime);
	for_each_possible_cpu(x) {
		user = user + kcpustat_cpu(x).cpustat[CPUTIME_USER];
		nice = nice + kcpustat_cpu(x).cpustat[CPUTIME_NICE];
		system = system + kcpustat_cpu(x).cpustat[CPUTIME_SYSTEM];
		idle = idle + get_idle_time(x);
		iowait = iowait + get_iowait_time(x);
		irq = irq + kcpustat_cpu(x).cpustat[CPUTIME_IRQ];
		softirq = softirq + kcpustat_cpu(x).cpustat[CPUTIME_SOFTIRQ];
		steal = steal + kcpustat_cpu(x).cpustat[CPUTIME_STEAL];
		guest = guest + kcpustat_cpu(x).cpustat[CPUTIME_GUEST];
		guest_nice = guest_nice + kcpustat_cpu(x).cpustat[CPUTIME_GUEST_NICE];
		sum = sum + kstat_cpu_irqs_sum(x);		
	}

	tiempo = user + idle + system + iowait + irq + softirq + steal + guest + guest_nice + sum;
	porcram = ((i.freeram*4)/(i.totalram*4))*100;
  	porctiempo = (((tiempo)/(idle))-1)*100;
	seq_printf(m,"{\"RAMLibre\":%8lu,""\"PorcentajeRAMLibre\":%8lu,", i.freeram*4,i.totalram*4);
	seq_printf(m, "\"CPUUsado\":");
	seq_put_decimal_ull(m, ' ', cputime64_to_clock_t(tiempo));
	seq_printf(m, ",\"PorcentajeCPUUsado\":");
	seq_put_decimal_ull(m, ' ', cputime64_to_clock_t(idle));
	seq_printf(m, "}\n");
        return 0;
    }

    static void __exit final(void)
    {   
        printk(KERN_INFO "Cleaning Up.\n");
    }

    static int meminfo_proc_open(struct inode *inode, struct file *file)
    {
        return single_open(file, meminfo_proc_show, NULL);
    }

    static const struct file_operations meminfo_proc_fops = {
        .open       = meminfo_proc_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
    };

    static int __init inicio(void)
    {
        proc_create("cpu_info", 0, NULL, &meminfo_proc_fops);
        return 0;
    }

    module_init(inicio);
    module_exit(final);

	MODULE_AUTHOR("juan jose lemus vasquez");
    MODULE_DESCRIPTION("201404412");
    MODULE_LICENSE("GPL");

