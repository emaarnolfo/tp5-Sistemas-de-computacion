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

#define DEVICE_NAME "ultrasonic"
#define TRIGGER_PIN 24        //GPIO 24
#define ECHO_PIN 23           //GPIO 23

#define MAX_BUFFER_LENGTH 7
#define TRIGGER_HIGH_TIME_uS 10
#define SPEED_OF_SOUND_M_S 343

static dev_t first;
static struct cdev c_dev;
static struct class *cl;

static uint8_t echo_pin = 23;
static uint8_t trig_pin = 24;


static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "ultrasonic: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "ultrasonic: close()\n");
    return 0;
}

// The read function is used to read the distance from the sensor

static ssize_t my_read(
    struct file *f, char __user *buf, size_t len, loff_t *off)
{
    long timeout_us;
    unsigned long elapsed_us;

    ktime_t timeout, start, finish;
    
    printk(KERN_INFO "ultrasonic: read()");

    if (*off) {
        return 0;
    }

    pr_info("Attempting to read sensor\n");

    if (echo_pin >= 100 || trig_pin >= 100)
    {
        pr_err("GPIO pins have not been defined yet\n");
        return 0;
    }

    // Send trugger pulse
    gpio_set_value(trig_pin, 1);
    udelay(TRIGGER_HIGH_TIME_uS);
    gpio_set_value(trig_pin, 0);

    // Pulse round trip is a maximum of 8 meters
    // 8 pulses are required
    // Add a 200% margin for error just in case and because it doesn't
    // work without it.

    // This waiting is blocking but seems like the most accurate way

    timeout_us = 1e9;

    timeout = ktime_add_us(ktime_get(), timeout_us);

    // Look at timers maybe
    pr_info("Waiting for pin to go high");
    while (!gpio_get_value(echo_pin))
    {
        if (ktime_compare(ktime_get(), timeout) >= 0)
        {
            elapsed_us = ULONG_MAX;
            pr_err("Timeout reading the sensor, no object detected\n");
            goto timeout;
        }
    }
    start = ktime_get();
    while (gpio_get_value(echo_pin))
    {
        if (ktime_compare(ktime_get(), timeout) >= 0)
        {
            elapsed_us = ULONG_MAX;
            pr_err("Timeout reading the sensor, no object detected\n");
            goto timeout;
        }
    }


    finish = ktime_get();
    elapsed_us = (unsigned long)ktime_us_delta(finish, start);

    pr_info("Read the sensor, got a total round trip time of %lu us\n", elapsed_us);

timeout:
    len = sizeof(unsigned long);

    if (copy_to_user(buf, &elapsed_us, len))
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
};

static int __init ultrasonic_init(void)
{
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "ultrasonic: Registrado exitosamente..!!\n");

    if ((ret = alloc_chrdev_region(&first, 0, 1, DEVICE_NAME)) < 0)
    {
        return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, "ultrasonic_drive")))
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

    // Configurar los pines GPIO
    ret = gpio_request_one(TRIGGER_PIN, GPIOF_OUT_INIT_LOW, "trigger_pin");
    if (ret < 0)
    {
        printk(KERN_ALERT "ultrasonic: Error al solicitar el pin del disparador del sensor ultrasónico\n");
        return ret;
    }

    ret = gpio_request_one(ECHO_PIN, GPIOF_IN, "echo_pin");
    if (ret < 0)
    {
        printk(KERN_ALERT "ultrasonic: Error al solicitar el pin del eco del sensor ultrasónico\n");
        gpio_free(TRIGGER_PIN);
        return ret;
    }

    return 0;
}

static void __exit ultrasonic_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(TRIGGER_PIN);
    gpio_free(ECHO_PIN);
    printk(KERN_INFO "ultrasonic: dice Adios mundo cruel..!!\n");
}

module_init(ultrasonic_init);
module_exit(ultrasonic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SdComp: Windows98");
MODULE_DESCRIPTION("Controlador del sensor ultrasónico");
