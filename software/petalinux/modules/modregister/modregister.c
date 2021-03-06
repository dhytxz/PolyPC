#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/kernel.h> 
#include <linux/ioctl.h>
#include <linux/sched.h>
 

#include <asm/uaccess.h> 
#include <asm/io.h>

#include "modregister.h"

extern void hapara_req_lock(unsigned int num);
extern void hapara_rel_lock(unsigned int num);

static dev_t dev_num;
static struct class *cl;
static struct hapara_register *hapara_registerp;
/* Only tid need to be matched
static loff_t search(struct hapara_register *dev, loff_t offset, uint8_t target, loff_t *pre)
*/
// Workable codes for singly linked list

// static loff_t search(struct hapara_register *dev, loff_t offset, uint32_t target, loff_t *pre)
// {
// #ifdef __REGISTER_DEBUG__
//     printk(KERN_DEBUG "%s@%s: Enter.\n", __func__, MODULE_NAME);
//     printk(KERN_DEBUG "%s@%s: offset = %d, target = %d.\n", __func__, MODULE_NAME, 
//            (int)offset, (int)target);
// #endif
//     struct hapara_thread_struct *thread_info = (struct hapara_thread_struct *)dev->mmio;
//     struct hapara_thread_struct *thread_head = (struct hapara_thread_struct *)dev->mmio;
//     *pre = -EINVAL;
//     int i = 0;
//     int isBegin = VALID;
//     while (!((i >= MAX_SLOT) || 
//            ((thread_info->isValid == INVALID) && 
//             (isBegin == INVALID)))) { 
//         switch (offset) {
//         /*
//         case OFF_VALID:
//             if (thread_info->valid == target)
//                 return i;
//             break;
//         case OFF_TYPE:
//             if (thread_info->type == target)
//                 return i;
//             break;
//         case OFF_PRIORITY:
//             if (thread_info->priority == target)
//                 return i;
//             break;
//         case OFF_NEXT:
//             if (thread_info->next == target)
//                 return i;
//             break;
//         */
//         case OFF_TID:
//             if (thread_info->tid == target)
//                 return i;
//             break;
//         default: 
//             return -EINVAL;
//         }
//         if (thread_info->isValid == VALID) {
//             *pre = i;
//             isBegin = INVALID;
//         }
//         if (thread_info->next != INVALID)
//             i = thread_info->next;
//         else
//             i++;
// #ifdef __REGISTER_DEBUG__
//         printk(KERN_DEBUG "%s@%s: Curr i = %d.\n", __func__, MODULE_NAME, i);
// #endif
//         thread_info = thread_head + i;
//     }
//     return -EINVAL;
// }

// static loff_t find_slot(struct hapara_register *dev)
// {
//     struct hapara_thread_struct *thread_info = (struct hapara_thread_struct *)dev->mmio;
//     int i = 0;
//     while (i < MAX_SLOT) {
//         if (thread_info->isValid == INVALID)
//             return i;
//         else {
//             i++;
//             thread_info++;
//         }
//     }
//     return -EINVAL;
// }

// static loff_t add(struct hapara_register *dev, struct hapara_thread_struct *buf)
// {
//     struct hapara_thread_struct *thread_info = (struct hapara_thread_struct *)dev->mmio;
//     loff_t off = find_slot(dev);
//     if (off == -EINVAL)
//         return -EINVAL;
//     if (copy_from_user(thread_info + off, (char *)buf, sizeof(struct hapara_thread_struct)))
//         return -EINVAL;
//     if (off != 0) {
//         (thread_info + off)->next = (thread_info + off - 1)->next;
//         (thread_info + off - 1)->next = INVALID;
//         if ((thread_info + off)->next == off + 1)
//             (thread_info + off)->next = INVALID;
//     }
//     return off;
// }

// static loff_t del(struct hapara_register *dev, loff_t offset, uint32_t target)
// {
//     struct hapara_thread_struct *thread_info = (struct hapara_thread_struct *)dev->mmio;
//     loff_t pre;
//     loff_t off = search(dev, offset, target, &pre);
// #ifdef __REGISTER_DEBUG__
//     printk(KERN_DEBUG "%s@%s: Search ret: %d.\n", __func__, MODULE_NAME, (int)off);
//     printk(KERN_DEBUG "%s@%s: Search pre: %d.\n", __func__, MODULE_NAME, (int)pre);
// #endif
//     if (off == -EINVAL) 
//         return -EINVAL;
//     if (pre == -EINVAL)
//         (thread_info + off)->isValid = INVALID;
//     else if ((thread_info + off)->next == INVALID) {
//         (thread_info + off)->isValid = INVALID;
//         (thread_info + pre)->next = off + 1;
//     } else {
//         (thread_info + off)->isValid = INVALID;
//         (thread_info + pre)->next = (thread_info + off)->next;
//     }
//     return off;
// }

/* Doubly linked codes
 */
static int add(struct hapara_register *dev, struct hapara_thread_struct *new_node)
{
    struct hapara_thread_struct *curr;
    struct hapara_thread_struct *sbase = 
        (struct hapara_thread_struct *)dev->mmio;

    int ret = -1;
    for_each_slot(curr, dev->mmio)
    {
        if (curr->isValid == INVALID) {
            if (copy_from_user(curr, 
                               (char *)new_node, 
                               sizeof(struct hapara_thread_struct)))
                return -EINVAL;            
            curr->isValid = VALID;
            curr->next = (curr - 1)->next;
            curr->prev = curr - sbase - 1;
            (curr - 1)->next = curr - sbase;
            curr->tid = current->pid;
            ret = curr - sbase;
            break;
        }
    }
    return ret;
}

static int del(struct hapara_register *dev, int off)
{
    struct hapara_thread_struct *curr = 
        (struct hapara_thread_struct *)dev->mmio + off;
    struct hapara_thread_struct *sbase = 
        (struct hapara_thread_struct *)dev->mmio;
    (sbase + curr->prev)->next = curr->next;
    (sbase + curr->next)->prev = curr->prev;
    curr->isValid = INVALID;
    return off;
}

static int search_del(struct hapara_register *dev, pid_t tid)
{
    struct hapara_thread_struct *curr;
    struct hapara_thread_struct *sbase = 
        (struct hapara_thread_struct *)dev->mmio;

    int ret = -1;
    for_each_valid(curr, dev->mmio)
    {
        if (curr->tid == tid) {
            (sbase + curr->prev)->next = curr->next;
            (sbase + curr->next)->prev = curr->prev;
            curr->isValid = INVALID;
            ret = curr - sbase;
            break;
        }
    }
    return ret;
}

static void print_list(struct hapara_register *dev)
{
    struct hapara_thread_struct *thread_info;
    struct hapara_thread_struct *base = 
        (struct hapara_thread_struct *)dev->mmio;
    for_each_valid(thread_info, dev->mmio)
    {
        printk(KERN_DEBUG "----------------------------------------\n");
        printk(KERN_DEBUG "ID:      = %d\n", thread_info - base);
        printk(KERN_DEBUG "isValid  = %d\n", thread_info->isValid);
        printk(KERN_DEBUG "priority = %d\n", thread_info->priority);
        printk(KERN_DEBUG "next     = %d\n", thread_info->next);
        printk(KERN_DEBUG "tid      = %d\n", thread_info->tid);
        printk(KERN_DEBUG "cur_group_id 0 = %d\n", thread_info->cur_group_id.id0);
        printk(KERN_DEBUG "cur_group_id 1 = %d\n", thread_info->cur_group_id.id1);
        printk(KERN_DEBUG "group_size 0   = %d\n", thread_info->group_size.id0);
        printk(KERN_DEBUG "group_size 1   = %d\n", thread_info->group_size.id1);
        printk(KERN_DEBUG "group_num 0    = %d\n", thread_info->group_num.id0);
        printk(KERN_DEBUG "group_num 1    = %d\n", thread_info->group_num.id1);
        printk(KERN_DEBUG "elf_info main_addr   = 0x%08X\n", thread_info->elf_info.main_addr);
        printk(KERN_DEBUG "elf_info stack_addr  = 0x%08X\n", thread_info->elf_info.stack_addr);
        printk(KERN_DEBUG "elf_info thread_size = 0x%08X\n", thread_info->elf_info.thread_size);
        printk(KERN_DEBUG "elf_info ddr_addr    = 0x%08X\n", thread_info->elf_info.ddr_addr);
        printk(KERN_DEBUG "elf_info DMA_size    = 0x%08X\n", thread_info->elf_info.DMA_size);
        printk(KERN_DEBUG "---------------------------------------\n");
    }
}
static int register_open(struct inode *inode, struct file *filp)
{
    filp->private_data = hapara_registerp;
    hapara_req_lock(REG_MUTEX);
    return 0;
}

static int register_release(struct inode *inode, struct file *filp)
{
    hapara_rel_lock(REG_MUTEX);
    return 0;
}

static ssize_t register_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct hapara_register *dev = filp->private_data;
    if (copy_to_user(buf, dev->mmio + p, count))
        return -EFAULT;
    else {
        *ppos += count;
        ret = count;
    }
    return ret;
}

static ssize_t register_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct hapara_register *dev = filp->private_data;
    if (copy_from_user(dev->mmio + p, buf, count)) 
        return -EFAULT;
    else {
        *ppos += count;
        ret = count;
    }
    return ret;
}

static loff_t register_llseek(struct file *filp, loff_t offset, int orig)
{   //0: set, 1: cur, 2: end
    loff_t ret = 0;
    switch (orig) {
    case 0:
        if (offset < 0) {
            ret = -EINVAL;
            break;
        }
        filp->f_pos = (unsigned int)offset;
        ret = filp->f_pos;
        break;
    case 1:
        if ((filp->f_pos + offset) < 0) {
            ret = -EINVAL;
            break;
        }
        filp->f_pos += offset;
        ret = filp->f_pos;
        break;
    default:
        ret = -EINVAL;
        break;
    }
    return ret;
}

static int register_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err = 0;
    int ret = -1;
    int location = 0;
    int off = 0;

    struct hapara_register *dev = filp->private_data;

/*   
    if (_IO_TYPE(cmd) != REG_MAGIC)
        return -EINVAL;
    if (_IO_NR(cmd) > REG_MAX)
        return -EINVAL;

    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    if (err) 
        return -EINVAL;
*/

    switch (cmd) {
    case REG_CLR:
        // printk(KERN_DEBUG "modregister:enter REG_CLR.\n");
        memset(dev->mmio, 0, SCHE_SIZE);
        break;
    case REG_ADD:
        // printk(KERN_DEBUG "modregister:enter reg+REG_ADD\n");
        location = add(dev, (struct hapara_thread_struct __user *)arg);
        if (location == -EINVAL)
            ret = -EINVAL;
        else
            ret = location;
        break;
    case REG_DEL:
        if (get_user(off, (int *)arg)) {
            // printk(KERN_DEBUG "modregister:reg_del:get_user failed.\n", off);
            return -EINVAL;
        } 
        // printk(KERN_DEBUG "modregister:reg_del:off = 0x%d\n", off);
        ret = del(dev, off);
        break;
    case REG_SEARCH_DEL:
        // printk(KERN_DEBUG "modregister:enter REG_SEARCH_DEL.\n");
        if (get_user(off, (int *)arg))
            return -EINVAL;
        location = search_del(dev, off);
        if (location == -EINVAL)
            ret = -EINVAL;
        else
            ret = location;
        break;
    case REG_PRINT_LIST:
        // printk(KERN_DEBUG "modregister:enter REG_PRINT_LIST.\n");
        print_list(dev);
        break;
    default:
        // printk(KERN_DEBUG "modregister:enter default.\n");
        ret = -EINVAL;
        break;
    }
    return ret;
}

static const struct file_operations register_fops = {
    .owner = THIS_MODULE,
    .open = register_open,
    .release = register_release,
    .llseek = register_llseek,
    .read = register_read,
    .write = register_write,
    .unlocked_ioctl = register_ioctl,
};

static int __init register_init(void)
{
    int ret;
    ret = alloc_chrdev_region(&dev_num, 0, 1, MODULE_NAME);
    if (ret < 0)
        goto failure_dev_reg;
    cl = class_create(THIS_MODULE, MODULE_NAME);
    if (cl == NULL) 
        goto failure_cl_cr;
    if (device_create(cl, NULL, dev_num, NULL, MODULE_NAME) == NULL)
        goto failure_dev_cr;
    hapara_registerp = kzalloc(sizeof(struct hapara_register), GFP_KERNEL);
    if (!hapara_registerp)
        goto failure_alloc;       
    cdev_init(&hapara_registerp->cdev, &register_fops);
    hapara_registerp->cdev.owner = THIS_MODULE;
    if (cdev_add(&hapara_registerp->cdev, dev_num, 1) == -1)
        goto failure_alloc;
#ifdef __REGISTER_DDR_MEM__
    hapara_registerp->mmio = kzalloc(SCHE_SIZE, GFP_KERNEL);
    if (!hapara_registerp->mmio) 
        goto failure_alloc;
#else
    hapara_registerp->mmio = ioremap(SCHE_BASE_ADDR, SCHE_SIZE);
#endif


    struct hapara_thread_struct * sb = 
                (struct hapara_thread_struct *)(hapara_registerp->mmio);
    (sb + DUMMY)->isValid = VALID;
    //initialize dusmmy head
    //next, prev, priority, isValid
    // //initialize the HTDT list
    // uint32_t i;
    // val = 0x02000000;
    // for (i = 1; i < MAX_SLOT - 1; i++) {
    //     (sb + i)->zero = val;
    //     val += 0x01010000;
    // }
    // val = 0x00FE0000;
    // (sb + MAX_SLOT - 1)->zero = val;
    // (sb + DUMMY)->isValid = VALID;

    return 0;

    failure_alloc:
    device_destroy(cl, dev_num);

    failure_dev_cr:
    class_destroy(cl);

    failure_cl_cr:
    unregister_chrdev_region(dev_num, 1);

    failure_dev_reg:
    return -1;   
}

static int __exit register_exit(void)
{
    cdev_del(&hapara_registerp->cdev);
    device_destroy(cl, dev_num);
    class_destroy(cl);
#ifdef __REGISTER_DDR_MEM__
    kfree(hapara_registerp->mmio);
#endif
    kfree(hapara_registerp);
    iounmap(hapara_registerp->mmio);
    unregister_chrdev_region(dev_num, 1);
    return 0;
}

module_init(register_init);
module_exit(register_exit);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("HaPara: Driver for host threads to register.");
