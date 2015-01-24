#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static char *whom = "world";
static int howmany = 1;
int i = 0;

module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

static int __init  hello_init(void)
{
	printk(KERN_ALERT "hello world!\n");
        for(i = 0; i < howmany; i++)
                printk(KERN_ALERT "hello %s\n", whom);

	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world!\n");
}



module_init(hello_init);
module_exit(hello_exit);
