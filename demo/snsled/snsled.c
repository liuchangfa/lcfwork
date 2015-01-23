#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/system.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */

//#include <asm/semaphore.h> /* semaphore */
#include <linux/semaphore.h>  
#include <linux/device.h>   /*class_create*/  

#include "snsled.h"		/* local definitions */


/*
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/leds.h>
#include <linux/leds-mt65xx.h>
#include <linux/workqueue.h>
#include <linux/wakelock.h>
#include <linux/slab.h>

#include <cust_leds.h>*/

#if defined (CONFIG_ARCH_MT6573)
#include <mach/mt6573_pwm.h>
#include <mach/mt6573_gpio.h>
#include <mach/pmu6573_sw.h>

#elif defined (CONFIG_ARCH_MT6516)
#include <mach/mt6516_pwm.h>
#include <mach/mt6516_gpio.h>

#endif


/*====macros====*/
#define BUF_SIZE (64)

#define SNS_LED_CONTROL_LINE				GPIO99	//GPIO39
#define SNS_LED_CONTROL_LINE_GPIO_MODE		GPIO_MODE_00	
#define SNS_LED_CONTROL_LINE_PWM_MODE		GPIO_MODE_01



/*====declares====*/
ssize_t snsled_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t snsled_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
long snsled_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
int snsled_open(struct inode *inode, struct file *filp);
int snsled_release(struct inode *inode, struct file *filp);

int snsled_turn_on(void);
int snsled_turn_off(void);
int snsled_set_pwm(int arg);


/*====global====*/
static int g_snsled_major = 0;
static int g_snsled_minor = 0;

struct snsled_cntx *g_snsled_ptr = NULL;

struct class *g_snsled_class = 0;

static struct file_operations g_snsled_fops = {
    .owner = THIS_MODULE,
    .read = snsled_read,
    .write = snsled_write, 
    .unlocked_ioctl = snsled_unlocked_ioctl,
    .open = snsled_open,
    .release = snsled_release,
};


/*====implements====*/
ssize_t snsled_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
#if 1
    printk(KERN_ALERT "Snsled read.\n");
#else
    char tmp_buf[512] = {0};
    int len =sprintf(tmp_buf, "snsled read.\n");
    if (copy_to_user(buf, tmp_buf, count)) {
        //do nothing
	}
#endif
    return 0;
}

ssize_t snsled_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	printk(KERN_ALERT "Snsled write.\n");
    return count;
}

long snsled_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    int err = 0;
    long retval = 0;
	
	printk(KERN_ALERT "Snsled ioctl:cmd=%d, arg=%d.\n", cmd, arg);
	
	/*
	//extract the type and number bitfields, and don't decode
	//wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	if (_IOC_TYPE(cmd) != SNSLED_IOC_MAGIC) return -ENOTTY;
	//if (_IOC_NR(cmd) > SNSLED_IOC_MAXNR) return -ENOTTY;

	//to verify *arg is in user space
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;
       */
    switch(cmd)
    {
        case SNSLED_IO_ON:
            printk(KERN_ALERT "Snsled ioctl:on.\n");
			//if(mt_set_gpio_out(SNS_LED_CONTROL_LINE,GPIO_OUT_ONE)){printk("Snsled set gpio failed!! \n");}
			snsled_turn_on();
            break;
			
        case SNSLED_IO_OFF:
            printk(KERN_ALERT "Snsled ioctl:off.\n");
			//if(mt_set_gpio_out(SNS_LED_CONTROL_LINE,GPIO_OUT_ZERO)){printk("Snsled set gpio failed!! \n");}
			snsled_turn_off();
            break;
			
        case SNSLED_IOW_PWM:            
			printk(KERN_ALERT "Snsled ioctl:set pwm, arg=%d.\n", arg);
            //retval = __get_user(g_snsled_ptr->r1, (int __user *)arg);
            snsled_set_pwm((int __user *)arg);
            break;

        case SNSLED_IOR_PWM:   
			#if 0
            retval = __put_user(g_snsled_ptr->r1, (int __user *)arg);
            printk(KERN_ALERT "Snsled ioctl:read r1:%i.\n", g_snsled_ptr->r1);
			#endif
			printk(KERN_ALERT "Snsled ioctl:read pwm -- not configured yet.\n");
            break;

        default:
            printk(KERN_ALERT "Snsled ioctl:you got the wrong command.\n");
            break;
    }
    
    return retval;
}

int snsled_open(struct inode *inode, struct file *filp) {
	printk(KERN_ALERT "Snsled: snsled_open.\n");
#if 0	
    if(mt_set_gpio_mode(SNS_LED_CONTROL_LINE,SNS_LED_CONTROL_LINE_GPIO_MODE)){printk("Snsled set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(SNS_LED_CONTROL_LINE,GPIO_DIR_OUT)){printk("Snsled set gpio dir failed!! \n");}
    if(mt_set_gpio_out(SNS_LED_CONTROL_LINE,GPIO_OUT_ONE)){printk("Snsled set gpio failed!! \n");}
#endif	
    return 0;
}

int snsled_release(struct inode *inode, struct file *filp) {
	printk(KERN_ALERT "Snsled: snsled_release.\n");
#if 0	
    if(mt_set_gpio_mode(SNS_LED_CONTROL_LINE,SNS_LED_CONTROL_LINE_GPIO_MODE)){printk("Snsled set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(SNS_LED_CONTROL_LINE,GPIO_DIR_OUT)){printk("Snsled set gpio dir failed!! \n");}
    if(mt_set_gpio_out(SNS_LED_CONTROL_LINE,GPIO_OUT_ZERO)){printk("Snsled set gpio failed!! \n");}
#endif	
    return 0;
}

int snsled_turn_on(void) {
	printk(KERN_ALERT "Snsled: snsled_turn_on.\n");
    if(mt_set_gpio_mode(SNS_LED_CONTROL_LINE,SNS_LED_CONTROL_LINE_PWM_MODE)){printk("Snsled set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(SNS_LED_CONTROL_LINE,GPIO_DIR_OUT)){printk("Snsled set gpio dir failed!! \n");}
    if(mt_set_gpio_out(SNS_LED_CONTROL_LINE,GPIO_OUT_ZERO)){printk("Snsled set gpio failed!! \n");}
	return 0;
}

int snsled_turn_off(void) {
	printk(KERN_ALERT "Snsled: snsled_turn_off.\n");
    if(mt_set_gpio_mode(SNS_LED_CONTROL_LINE,SNS_LED_CONTROL_LINE_GPIO_MODE)){printk("Snsled set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(SNS_LED_CONTROL_LINE,GPIO_DIR_OUT)){printk("Snsled set gpio dir failed!! \n");}
    if(mt_set_gpio_out(SNS_LED_CONTROL_LINE,GPIO_OUT_ZERO)){printk("Snsled set gpio failed!! \n");}
	return 0;
}

//for old mode
/**
struct _PWM_OLDMODE_REGS {
	U16 IDLE_VALUE; //0
	U16 GUARD_VALUE;	//0
	U16 GDURATION;	//~
	U16 WAVE_NUM;	//0
	U16 DATA_WIDTH;	//high level, 13bits, 0~8191
	U16 THRESH;	//t
}PWM_MODE_OLD_REGS;
**/
int snsled_set_pwm(int arg) {
	struct pwm_spec_config pwm_setting;
	pwm_setting.pwm_no = PWM1;
	printk(KERN_ALERT "Snsled: snsled_open begin.\n");

	
	pwm_setting.mode = PWM_MODE_OLD;
	pwm_setting.clk_div = CLK_DIV16;//CLK_DIV128;
	pwm_setting.clk_src = PWM_CLK_OLD_MODE_32K;	
	pwm_setting.PWM_MODE_OLD_REGS.IDLE_VALUE = 0;
	pwm_setting.PWM_MODE_OLD_REGS.GUARD_VALUE = 0;
	pwm_setting.PWM_MODE_OLD_REGS.GDURATION = 8100;
	pwm_setting.PWM_MODE_OLD_REGS.WAVE_NUM = 0;
	pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = 8100;
	pwm_setting.PWM_MODE_OLD_REGS.THRESH = 8100;

	pwm_set_spec_config(&pwm_setting);	
	printk(KERN_ALERT "Snsled: snsled_open done.\n");

	return 0;
}




//alloc device major
static int vircdex_alloc_major(void) {
    dev_t devt = 0;
    int result = 0;
    
    result = alloc_chrdev_region(&devt, g_snsled_minor, SNSLED_NUM, SNSLED_NODE_NAME);
    g_snsled_major = MAJOR(devt);
    
    return result;
}
static int snsled_release_major(void) {
    dev_t devt = MKDEV(g_snsled_major, g_snsled_minor);
    unregister_chrdev_region(devt, 1);
    return 0;
}

static int snsled_setup_dev(struct snsled_cntx *dev) {
    int err, devno = MKDEV(g_snsled_major, g_snsled_minor);
    
    cdev_init(&(dev->cdev), &g_snsled_fops);
    dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&dev->cdev, devno, 1);
    if(err){   
        return err;
    }
    
    //init_MUTEX(&(dev->sem));
    sema_init(&(dev->sem), 1);
    
    return 0;
}
static int snsled_unsetup_dev(struct snsled_cntx *dev) {
    cdev_del(&(dev->cdev));
    return 0;
}

static int snsled_create_devfiles(dev_t devt) {//, const struct device_attribute *attr) {
    int err = -1;    
    struct device *dev = NULL;
                          
    g_snsled_class = class_create(THIS_MODULE, SNSLED_CLASS_NAME);
    if(IS_ERR(g_snsled_class)) {  
        err = PTR_ERR(g_snsled_class);  
        printk(KERN_ALERT "Failed to create class.\n");  
        goto CLASS_CREATE_ERR;  
    }
    
    dev = device_create(g_snsled_class, NULL, devt, NULL, SNSLED_DEVICE_NAME);
    //dev = device_create(hello_class, NULL, dev, "%s", HELLO_DEVICE_FILE_NAME);  
    //device_create( my_class, NULL, MKDEV(hello_major, 0), "hello" "%d", 0 );
    //dev = device_create(g_snsled_class, NULL, MKDEV(MYDRIVER_Major, 0), NULL, DEVICE_NAME);
    if(IS_ERR(dev)) {  
        err = PTR_ERR(dev);  
        printk(KERN_ALERT "Failed to create device.\n");  
        goto DEVICE_CREATE_ERR;  
    }
    
    /*err = device_create_file(dev, attr);  
    if(err < 0) {  
        printk(KERN_ALERT"Failed to create attribute file.");                  
        goto DEVICE_CREATE_FILE_ERR;  
    }*/          
    printk(KERN_ALERT "seems ok.\n"); //zmk@@debug    
    
    return 0;
    
DEVICE_CREATE_FILE_ERR:
    device_destroy(g_snsled_class, devt);
DEVICE_CREATE_ERR:
    class_destroy(g_snsled_class);             
CLASS_CREATE_ERR:  
    return err; 
}
static int snsled_delete_devfiles(dev_t devt) {
    device_destroy(g_snsled_class, devt);
    class_destroy(g_snsled_class);
    //device_remove_file(dev, attr);
    return 0;
}

static int snsled_read_proc(char *buf, char **start, off_t offset,
                            int count, int *eof, void *data)
{
    int len =sprintf(buf, "snsled read proc.\n");
    return len;
}
static int snsled_create_proc_file(void) {
    struct proc_dir_entry *entry = NULL;
 
    entry = create_proc_read_entry(SNSLED_PROC_NAME, 0,
                            NULL, snsled_read_proc,
                            NULL);
    if(entry)
    {
        return 0;
    }
    else                            
    {                        
        return -1;                       
    }                       
}
static int snsled_delete_proc_file(void) {
    remove_proc_entry(SNSLED_PROC_NAME, NULL);
    return 0;
}

MODULE_LICENSE("GPL");

static int snsled_init(void) {
    int err = -1;
    dev_t devt = 0;
    
    //[1] alloc node number
    err = vircdex_alloc_major();
    if(0 > err)
    {
        printk(KERN_ALERT"alloc major failed.\n");
        goto ALLOC_MAJOR_ERR;
    }
    devt = MKDEV(g_snsled_major, g_snsled_minor);
    
    //[2] device object init    
    g_snsled_ptr = kmalloc(sizeof(struct snsled_cntx), GFP_KERNEL);  
    if(!g_snsled_ptr) {  
        err = -ENOMEM;  
        printk(KERN_ALERT"kmalloc failed.\n");  
        goto KMALLOC_ERR;  
    }
    memset(g_snsled_ptr, 0, sizeof(struct snsled_cntx));
    
    //[3] setup device
    err = snsled_setup_dev(g_snsled_ptr);
    if(0 > err)
    {
        printk(KERN_ALERT"device setup failed.\n");
        goto DEVICE_SETUP_ERR;
    }
    
    //[4] create files in directory "/dev/" and "/sys/" 
    ///err = snsled_create_devfiles(devt, attr);
    err = snsled_create_devfiles(devt);
    if(0 > err)
    {
        printk(KERN_ALERT"devfiles create failed.\n");
        goto DEVFILES_CREATE_ERR;
    }

    //[5] create proc file
    err = snsled_create_proc_file();
    if(0 > err)
    {
        printk(KERN_ALERT"proc file create failed.\n");
        goto PROC_FILE_CREATE_ERR;
    }
        
    return 0;
    
PROC_FILE_CREATE_ERR:
    snsled_delete_devfiles(devt);
DEVFILES_CREATE_ERR:
    snsled_unsetup_dev(g_snsled_ptr);
DEVICE_SETUP_ERR:
    kfree(g_snsled_ptr); 
ALLOC_MAJOR_ERR:
    snsled_release_major();
KMALLOC_ERR:
    return err;
}
static void snsled_exit(void) {
    dev_t devt = MKDEV(g_snsled_major, g_snsled_minor);
    
    snsled_delete_proc_file();
    snsled_delete_devfiles(devt);
    snsled_unsetup_dev(g_snsled_ptr);
    kfree(g_snsled_ptr); 
    snsled_release_major();
}

module_init(snsled_init);
module_exit(snsled_exit);
