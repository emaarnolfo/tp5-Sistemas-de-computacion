obj-m += driverGPIO.o
 
KDIR = -isystem /lib/modules/$(shell uname -r)/build/include
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean