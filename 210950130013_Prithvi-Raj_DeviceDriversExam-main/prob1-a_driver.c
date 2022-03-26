
#include<linux/init.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include<linux/fcntl.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>

#include "ioctl.h"
#define BLOCK_SIZE 512

char k_buff[BLOCK_SIZE];
size_t str_len=0;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Murthu");
MODULE_DESCRIPTION("driver that creates and calculates add,sub,div and mul\n");


static int my_open(struct inode *inode, struct file *file);

static int  my_release(struct inode *inode, struct file *file);


static ssize_t my_write(struct file *,const char *, size_t, loff_t *);

static ssize_t my_read(struct file *, char*, size_t, loff_t *);

static long int my_ioctl(struct file* filep,unsigned cmd,unsigned long argp);


dev_t calc_dev=0;
struct cdev kcalc_dev;

int operands[2]={0};
static size_t arr_index=0;

const char *device_path="exam_driver";


static struct file_operations fops={
    .owner      = THIS_MODULE,
    .read     = my_read,
    .write    = my_write,
    .open     = my_open,
    .release  = my_release,
};



static int __init exam_driver_initialization(void)
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

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO"Kernel Open Call\n");
    return 0;
}

static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
    ssize_t retvalue=-1;


    unsigned long result=0;

    //Returns number of bytes that could not be copied.
    result=copy_from_user(k_buff,buf,len);
    
    if(result == 0)
    {
        printk(KERN_INFO"Data Successfully Written \n");

        retvalue=len;
        return retvalue;
    }
    else if(result > 0)
    {
        retvalue=len-result;
        return retvalue;
    }
    printk(KERN_ALERT"Error writing Data\n");
    return retvalue;

}


static long int my_ioctl(struct file* filep,unsigned cmd,unsigned long argp)
{
     long result;
    size_t i=0;
    switch (cmd)
    {
    case string_reset:
        k_buff[0]='\0';
        break;
    case string_getlen:
        result=copy_to_user((int*)argp,(int*)&str_len,4);
        break;
    
    default:
        printk(KERN_ALERT"default\n");
        break;
    }
    return result;
}

static ssize_t my_read(struct file *filp, char* buf, size_t len, loff_t * off)
{
    ssize_t retvalue=-1;

    unsigned long result;

    if(*off + len > BLOCK_SIZE)
    {
        len=BLOCK_SIZE - *off;
    }
    

    printk(KERN_INFO "Kernel READ Call\n");
    
    /*Returns number of bytes that could not be copied.*/
    result=copy_to_user(buf,k_buff,len);

    printk("copied buffer %s\n",buf);

    
    if(result>=0)
    {
        retvalue=len-result;
        return retvalue;    
    }
    printk(KERN_ALERT "\n ERROR WRITING DATA TO USER\n ");
    return retvalue;
    

}


static int  my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO"resetting array index in release call\n");
    arr_index=0;
    return 0;
}


static void __exit exam_driver_release(void)
{
    
    unregister_chrdev_region(calc_dev,1);
    printk(KERN_INFO "unregistered character device successfully\n");
    cdev_del(&kcalc_dev);
    printk(KERN_INFO "deleted  character device from kernel\n");

}

module_init(exam_driver_initialization);
module_exit(exam_driver_release);
