
#include<linux/init.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include<linux/fcntl.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>

#include "_ioctl.h"

#define MAX_BLOCK_SIZE 512

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Murthu");
MODULE_DESCRIPTION("driver that creates and calculates add,sub,div and mul\n");


static int prob1b_open(struct inode *inode, struct file *file);

static int  prob1b_release(struct inode *inode, struct file *file);


static ssize_t prob1b_write(struct file *,const char *, size_t, loff_t *);

static ssize_t prob1b_read(struct file *, char*, size_t, loff_t *);

static long int prob1b_ioctl(struct file* filep,unsigned cmd,unsigned long argp);



static struct file_operations fops={
    .owner      = THIS_MODULE,
    .read     = prob1b_read,
    .write    = prob1b_write,
    .open     = prob1b_open,
    .release  = prob1b_release,
    .unlocked_ioctl  =prob1b_ioctl,
};

dev_t calc_dev=0;
struct cdev kcalc_dev;

static struct stats kernel_record;


const char *device_path="exam_driverprob1";


static int __init prob1b_driver_initialization(void)
{
    calc_dev = MKDEV(401,0);
    if(register_chrdev_region(calc_dev, 1,device_path) < 0)
    {
        printk(KERN_ALERT"\ncould not allocate device numbers\n");
        return -1;
    }
   
    cdev_init(&kcalc_dev,&fops);  //kcalc_dev to initialize with related file operations retuns nothing

    if(cdev_add(&kcalc_dev,calc_dev,1) < 0)
    {
        unregister_chrdev_region(calc_dev,1);
        printk(KERN_ALERT "\ncould not add devices to kernel\n");
        return -1;
    }

    printk(KERN_INFO "\nadded devices to kernel\n");

    
    return 0;
}

static int prob1b_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO"Kernel Open Call\n");
    
    return 0;
}

static ssize_t prob1b_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
    ssize_t retvalue=-1;


    unsigned long result=0;

    //Returns number of bytes that could not be copied.
    result=copy_from_user(kernel_record.k_buff,buf,len);

    if(result == 0)
    {
        printk(KERN_INFO"Data Successfully Written \n");
        printk(KERN_INFO"%s\n",kernel_record.k_buff);
        retvalue=len;
        kernel_record.r_w=1;
        kernel_record.size=retvalue;
        printk(KERN_INFO"size of kernel buffer %d\n",kernel_record.size);
        return retvalue;
    }
    else if(result > 0)
    {
        kernel_record.r_w=1;
        retvalue=len-result;
        kernel_record.size=retvalue;
        return retvalue;
    }
    printk(KERN_ALERT"Error writing Data\n");
    return retvalue;

}


static long int prob1b_ioctl(struct file* filep,unsigned cmd,unsigned long argp)
{
    long result;
    
    switch (cmd)
    {
    case get_kbuff_status:
        printk(KERN_INFO"kernel has %d\n%d\n%s\n",kernel_record.size,kernel_record.r_w,kernel_record.k_buff);
        result=copy_to_user((struct stats*)argp,(struct stats*)&kernel_record,sizeof(struct stats));
        break;
    default:
        printk(KERN_ALERT"default\n");
        break;
    }
    return result;
}

static ssize_t prob1b_read(struct file *filp, char* buf, size_t len, loff_t * off)
{
    ssize_t retvalue=-1;

    unsigned long result;

    if(kernel_record.size == 0)    
    {
        printk("no bytes to read\n");
        return 0;
    }

    printk(KERN_INFO "Kernel READ Call\n");
    
    /*Returns number of bytes that could not be copied.*/
    printk(KERN_INFO"size of kernel is %d\n",kernel_record.size);
    result=copy_to_user(buf,kernel_record.k_buff,kernel_record.size);
    kernel_record.r_w=0;
    kernel_record.size=0;

    printk("copied buffer %s\n",buf);

    
    if(result>=0)
    {
        retvalue=len-result;
        return retvalue;    
    }
    printk(KERN_ALERT "\n ERROR WRITING DATA TO USER\n ");
    return retvalue;
    

}


static int  prob1b_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO" release call\n");
    return 0;
}


static void __exit prob1b_driver_release(void)
{
    
    unregister_chrdev_region(calc_dev,1);
    printk(KERN_INFO "unregistered character device successfully\n");
    cdev_del(&kcalc_dev);
    printk(KERN_INFO "deleted  character device from kernel\n");

}

module_init(prob1b_driver_initialization);
module_exit(prob1b_driver_release);
