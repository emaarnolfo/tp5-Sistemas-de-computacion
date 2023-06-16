#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define DEVICE_NAME "joystick"

#define PIN0 2
#define PIN1 3
#define PIN2 4
#define PIN3 17
#define PIN4 27
#define PIN5 22
#define PIN6 10
#define PIN7 9
#define PIN8 11
#define PIN9 0

static dev_t first;
static struct cdev c_dev;
static struct class *cl;

struct gpio pins[] = {
        {PIN0, GPIOF_IN, "pin0"},
        {PIN1, GPIOF_IN, "pin1"},
        {PIN2, GPIOF_IN, "pin2"},
        {PIN3, GPIOF_IN, "pin3"},
        {PIN4, GPIOF_IN, "pin4"},
        {PIN5, GPIOF_IN, "pin5"},
        {PIN6, GPIOF_IN, "pin6"},
        {PIN7, GPIOF_IN, "pin7"},
        {PIN8, GPIOF_IN, "pin8"},
        {PIN9, GPIOF_IN, "pin9"}
};
    
static const int num_pines = sizeof(pins) / sizeof(pins[0]);

static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Joystick: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Joystick: close()\n");
    return 0;
}

static ssize_t my_read(
    struct file *f, char __user *buf, size_t len, loff_t *off)
{
    unsigned int value_joy = 0;
    
    printk(KERN_INFO "joystick: read()");

    if (*off) {
        return 0;
    }
    
    for (int i = 0; i < 10; i++) {
        value_joy |= gpio_get_value(pins[i].gpio) << i;
    }

    pr_info("Read the sensor, got a total round trip time of %lu us\n", value_joy);

    len = sizeof(unsigned int);
    
    //value_joy = 0

    if (copy_to_user(buf, &value_joy, len))
    {
        pr_err("Error copying elapsed time to user\n");
    }

    *off += len;
    return len;
}

static struct file_operations ultrasonic_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    //.write = my_write
};

static int __init joystick_init(void)
{
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "Joystick: Registrado exitosamente..!!\n");

    if ((ret = alloc_chrdev_region(&first, 0, 1, DEVICE_NAME)) < 0)
    {
        return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, "Joystick_drive")))
    {
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, DEVICE_NAME)))
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&c_dev, &ultrasonic_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }
    
    // Inicializacion de los 10 pines GPIO como entrada
    for (int i = 0; i < num_pines; i++) {
        ret = gpio_request_one(pins[i].gpio, pins[i].flags, pins[i].label);
        //printk(KERN_INFO "Pin solicitado correctamente: %s\n", pins[i].label);
        
        if (ret < 0) {
            pr_err("Error al solicitar el pin %d\n", pins[i].gpio);
            while (i > 0) {
                i--;
                gpio_free(pins[i].gpio);
            }
            return ret;
        }
    }
    
    printk(KERN_INFO "Joystick: Pines solicitados correctamente\n");
    
    return 0;
}

static void __exit joystick_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(PIN0);
    gpio_free(PIN1);
    gpio_free(PIN2);
    gpio_free(PIN3);
    gpio_free(PIN4);
    gpio_free(PIN5);
    gpio_free(PIN6);
    gpio_free(PIN7);
    gpio_free(PIN8);
    gpio_free(PIN9);
    printk(KERN_INFO "Joystick: dice Adios mundo cruel..!!\n");
}

module_init(joystick_init);
module_exit(joystick_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SdComp: Windows98");
MODULE_DESCRIPTION("Controlador de joystick");
