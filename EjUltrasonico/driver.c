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
#define TRIGGER_PIN (24)        //GPIO 24
#define ECHO_PIN (23)           //GPIO 23

static dev_t first;
static struct cdev c_dev;
static struct class *cl;

static int measurement_in_progress = 0;

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

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    int distance;
    ssize_t bytes_read;
    unsigned long pulse_duration;

    printk(KERN_INFO "ultrasonic: read()\n");

    if (measurement_in_progress)
    {
        return -EBUSY; // Si una medición ya está en progreso, retornar un error
    }

    // Disparar el pulso ultrasónico
    gpio_set_value_cansleep(TRIGGER_PIN, 1);
    udelay(10);
    gpio_set_value_cansleep(TRIGGER_PIN, 0);

    // Esperar por el eco
    //measurement_in_progress = 1;
    while (gpio_get_value(ECHO_PIN) == 0)
    {
        udelay(1);
    }
    udelay(2);

    unsigned long start_time = jiffies;
    while (gpio_get_value(ECHO_PIN) == 1)
    {
        udelay(1);
    }
    //measurement_in_progress = 0;
    unsigned long end_time = jiffies;

    pulse_duration = (end_time - start_time) * (1000000 / HZ);

    // Calcular la distancia en función de la velocidad del sonido y la duración del pulso
    // (ajusta el factor de conversión según tus necesidades)
    distance = pulse_duration * 17 / 1000;

    // Convertir la distancia a una cadena de caracteres
    char distance_str[16];
    snprintf(distance_str, sizeof(distance_str), "%d\n", distance);

    // Copiar la cadena de caracteres al espacio de usuario
    bytes_read = simple_read_from_buffer(buf, len, off, distance_str, strlen(distance_str));

    return bytes_read;
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
MODULE_AUTHOR("Cátedra Sistemas de Computación");
MODULE_DESCRIPTION("Controlador del sensor ultrasónico");
