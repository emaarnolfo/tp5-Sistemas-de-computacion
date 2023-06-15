/* Definiciones e includes necesarios para los drivers */
#define MODULE
#define __KERNEL__
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/malloc.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */


/* Declaracion de funciones de memoria.c */
int memoria_open(struct inode *inode, struct file *filp);
int memoria_release(struct inode *inode, struct file *filp);
ssize_t memoria_read(struct file *filp, char *buf,
size_t count, loff_t *f_pos);
ssize_t memoria_write(struct file *filp, char *buf,
size_t count, loff_t *f_pos);
void cleanup_module(void);

/* Estructura que declara las funciones tipicas */
/* de acceso a ficheros */
struct file_operations memoria_fops = {
    read: memoria_read,
    write: memoria_write,
    open: memoria_open,
    release: memoria_release
};

/* Variables globales del driver */
/* Numero mayor */
int memoria_major = 60;

/* Buffer donde guardar los datos */
char *memoria_buffer;

int init_module(void) {
    int result;

    /* Registrando dispositivo */
    result = register_chrdev(memoria_major, "memoria", &memoria_fops);

    if (result < 0) {
        printk("<1>memoria: no puedo obtener numero mayor %d\n",memoria_major);
        return result;
    }

    /* Reservando memoria para el buffer */
    memoria_buffer = kmalloc(1, GFP_KERNEL);

    if (!memoria_buffer) {
        result = -ENOMEM;
        goto fallo;
    }

    memset(memoria_buffer, 0, 1);
    printk("<1>Insertando modulo\n");
    return 0;

    fallo:
        cleanup_module();
        return result;
}


void cleanup_module(void) {
    /* Liberamos numero mayor */
    unregister_chrdev(memoria_major, "memoria");

    /* Liberamos memoria del buffer */
    if (memoria_buffer) {
        kfree(memoria_buffer);
    }

    printk("<1>Quitando modulo\n");
}

int memoria_open(struct inode *inode, struct file *filp) {
    /* Aumentamos la cuenta de uso */
    MOD_INC_USE_COUNT;
    /* Exito */
    return 0;
}

int memoria_release(struct inode *inode, struct file *filp) {
    /* Decrementamos la cuenta de uso */
    MOD_DEC_USE_COUNT;

    return 0;
}

ssize_t memoria_read(struct file *filp, char *buf,
    size_t count, loff_t *f_pos) {
    
    /* Transferimos datos al espacio de usuario */
    copy_to_user(buf,memoria_buffer,1);

    /* Cambiamos posición de lectura segun convenga */
    if (*f_pos == 0) {
        *f_pos+=1;
        return 1;
    } else {
        return 0;
    }
}

ssize_t memoria_write( struct file *filp, char *buf, size_t count, loff_t *f_pos) {
    char *tmp;
    tmp=buf+count-1;
    copy_from_user(memoria_buffer,tmp,1);
    return 1;
}