#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

MODULE_AUTHOR("JuanJose");
MODULE_DESCRIPTION("201404412");
MODULE_LICENSE("GPL");

char *aux;

static int meminfo_proc_show(struct seq_file *m, void *v)
{
    struct task_struct *task;
    for_each_process(task)
    {
        struct task_struct *task_child;
        struct list_head *list;
        
		switch(task->state){
			case 0:
				aux = "Ejecucion";
				break;
			case 1:
				aux = "Listo";
				break;
			case 2:
				aux = "Dormido";
				break;
			case 4:
				aux = "Zombie";
				break;
			case 8:
				aux = "Detenido";
				break;
			case 32:
				aux = "Espera Exclusiva";
				break;
		}
        seq_printf(m,"{\"nombre\": \"%s\", \"pid\": %d, \"estado\": \"%s\", \"usuario\":\"%d\", \"cpu\":\"%d\", \"ram\":\"%llu\"}\n", task->comm, task->pid, aux, task->cred->uid, task->cpuset_mem_spread_rotor, task->acct_vm_mem1);

        /*list_for_each(list,&task->children) {
            task_child=list_entry(list,struct task_struct,sibling);
             switch(task_child->state){
				case 0:
					aux = "Ejecucion";
					break;
				case 1:
					aux = "Listo";
					break;
				case 2:
					aux = "Dormido";
					break;
				case 4:
					aux = "Zombie";
					break;
				case 8:
					aux = "Detenido";
					break;
				case 32:
				aux = "Espera Exclusiva";
				break;
			}
            seq_printf(m,"{\"nombre\": \"%s\", \"pid\": %d, \"estado\": \"%c\", \"usuario\":\"%d\", \"cpu\":\"%d\", \"ram\":\"%llu\"}\n", task_child->comm, task_child->pid, aux, task->cred->uid, task->cpuset_mem_spread_rotor, task->acct_vm_mem1);
        }*/
    }
    return 0;
}

static void __exit final(void) //Salida de modulo
{   
    printk(KERN_INFO "Sistemas Operativos 1 Proc\n");
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
	printk(KERN_INFO "201404412\n");
    proc_create("proc_201404412", 0, NULL, &meminfo_proc_fops);
    return 0;
}


module_init(inicio);
module_exit(final);
