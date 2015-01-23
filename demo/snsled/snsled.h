#ifndef _SNSLED_H_
#define _SNSLED_H_

#define SNSLED_NUM (1)

#define SNSLED_CLASS_NAME   "snsled"
#define SNSLED_DEVICE_NAME  "snsled"
#define SNSLED_NODE_NAME    "snsled"
#define SNSLED_PROC_NAME    "snsled"

#define SNSLED_IOC_MAGIC   'k'

#define SNSLED_IO_ON        2323    //_IO(SNSLED_IOC_MAGIC, 0)
#define SNSLED_IO_OFF       2324    //_IO(SNSLED_IOC_MAGIC, 1)
#define SNSLED_IOW_PWM      2325    //_IOW(SNSLED_IOC_MAGIC, 2, int)
#define SNSLED_IOR_PWM      2326    //_IOR(SNSLED_IOC_MAGIC, 3, int)

struct snsled_cntx {
    int r1;
    struct semaphore sem; 
    struct cdev cdev;
};

#endif
