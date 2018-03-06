    #include <linux/fs.h>
    #include <linux/hugetlb.h>
    #include <linux/init.h>
    #include <linux/kernel.h>
    #include <linux/sched.h>
    #include <linux/module.h>
    #include <linux/seq_file.h>
    #include <linux/proc_fs.h>

    #include <linux/mm.h>
    #include <linux/mman.h>
    #include <linux/mmzone.h>

    #include <linux/quicklist.h>

    #include <linux/swap.h>
    #include <linux/vmstat.h>
    #include <linux/atomic.h>
    #include <asm/page.h>
    #include <asm/pgtable.h>

    #include <linux/kernel.h>
    #include <linux/module.h>
    #include <linux/cpumask.h>
    #include <linux/fs.h>
    #include <linux/init.h>
    #include <linux/interrupt.h>
    #include <linux/kernel_stat.h>
    #include <linux/proc_fs.h>
    #include <linux/sched.h>
    #include <linux/seq_file.h>
    #include <linux/slab.h>
    #include <linux/time.h>
    #include <linux/irqnr.h>
    #include <linux/cputime.h>
    #include <linux/tick.h>
    #include <asm/apic.h>

    #include <linux/smp.h>
    #include <linux/timex.h>
    #include <linux/string.h>
    #include <linux/seq_file.h>
    #include <linux/cpufreq.h>
    #include <linux/delay.h>

	static u64 get_idle_time(int cpu)
	{
		u64 idle, idle_usecs = -1ULL;

		if (cpu_online(cpu))
			idle_usecs = get_cpu_idle_time_us(cpu, NULL);

		if (idle_usecs == -1ULL)
			/* !NO_HZ or cpu offline so we can rely on cpustat.idle */
			idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
		else
			idle = usecs_to_cputime64(idle_usecs);
			//idle = idle_usecs * NSEC_PER_USEC;

		return idle;
	}

	static u64 get_iowait_time(int cpu)
	{
		u64 iowait, iowait_usecs = -1ULL;

		if (cpu_online(cpu))
			iowait_usecs = get_cpu_iowait_time_us(cpu, NULL);

		if (iowait_usecs == -1ULL)
			/* !NO_HZ or cpu offline so we can rely on cpustat.iowait */
			iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
		else
			//iowait = iowait_usecs * NSEC_PER_USEC;
			iowait = usecs_to_cputime64(iowait_usecs);


		return iowait;
	}


    static int meminfo_proc_show(struct seq_file *m, void *v)
    {
	/*
	 * Tagged format, for easy grepping and expansion.
	 */
	/***********************************************/
	/*			CPU			*/
	/***********************************************/
	int x;
	u64 user, nice, system, idle, iowait, irq, softirq, steal;
	u64 guest, guest_nice;
	u64 sum = 0;
	u64 sum_softirq = 0;

	u64 tiempo = 0;

	int64_t totalcpupercentage = 0;
	float pruebas;

	unsigned int per_softirq_sums[NR_SOFTIRQS] = {0};
	struct timespec64 boottime;

	user = nice = system = idle = iowait =
		irq = softirq = steal = 0;
	guest = guest_nice = 0;
	getboottime(&boottime);
	for_each_possible_cpu(x) {
		user += kcpustat_cpu(x).cpustat[CPUTIME_USER];
		nice += kcpustat_cpu(x).cpustat[CPUTIME_NICE];
		system += kcpustat_cpu(x).cpustat[CPUTIME_SYSTEM];
		idle += get_idle_time(x);
		iowait += get_iowait_time(x);
		irq += kcpustat_cpu(x).cpustat[CPUTIME_IRQ];
		softirq += kcpustat_cpu(x).cpustat[CPUTIME_SOFTIRQ];
		steal += kcpustat_cpu(x).cpustat[CPUTIME_STEAL];
		guest += kcpustat_cpu(x).cpustat[CPUTIME_GUEST];
		guest_nice += kcpustat_cpu(x).cpustat[CPUTIME_GUEST_NICE];
		sum += kstat_cpu_irqs_sum(x);
		
	}
tiempo = user+nice+system+idle+iowait+irq+softirq+steal+guest+guest_nice;
		seq_printf(m, "{\"tiempocpu\":");
		seq_put_decimal_ull(m, ' ', cputime64_to_clock_t(tiempo));
		seq_printf(m, ",\"idle\":");
		seq_put_decimal_ull(m, ' ', cputime64_to_clock_t(idle));
		seq_printf(m, "}\n");
        return 0;
    }

    static void __exit final(void) //Salida de modulo
    {   
        printk(KERN_INFO "Sistemas OPerativos 1\n");
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

    static int __init inicio(void) //Escribe archivo en /proc
    {
        proc_create("cpu_201404412", 0, NULL, &meminfo_proc_fops);
        return 0;
    }


    module_init(inicio);
    module_exit(final);

    MODULE_AUTHOR("JuanJose");
    MODULE_DESCRIPTION("201404412");
    MODULE_LICENSE("GPL");

