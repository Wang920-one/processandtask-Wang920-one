#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/sched/signal.h>

int simple_init(void)
{
	printk(KERN_INFO "Loading Module\n");
	
	struct task_struct *task;
	struct list_head *list;
	int count=0;
	
	list_for_each(list,&task->children) 
	{
		task=list_entry(list,struct task_struct,sibling);
		count++;
		printk(KERN_INFO "%d----->%s\n",task->pid,task->comm);
	}
	printk(KERN_INFO "the number of process is:%d\n",count);
    return 0;
}

void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");
}

module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

