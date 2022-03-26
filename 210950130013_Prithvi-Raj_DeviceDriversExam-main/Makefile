obj-m := prob1-b_driver.o

PWD := $(shell pwd)
KERNEL_DIR = /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KERNEL_DIR)  M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

nodes:
	sudo mknod /dev/prob1b_exam_driver c 401 0 
