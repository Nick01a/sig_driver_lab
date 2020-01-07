#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/kobject.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mykola Kyrychenko");
MODULE_DESCRIPTION("Linux driver segm");
MODULE_VERSION("0.1");

static unsigned int default_val = 8;
static unsigned int pin_list[7] = {11, 13, 15, 19, 21, 23, 29};

module_param(default_val, uint, 0660);

int set_default_value(int num){
    switch (num){
        case 0:
            gpio_set_value(11, 1);
            gpio_set_value(13, 1);
            gpio_set_value(21, 1);
            gpio_set_value(29, 1);
            gpio_set_value(23, 1);
            gpio_set_value(15, 1);
            gpio_set_value(19, 0);
            break;
        case 1:
            gpio_set_value(15, 1);
            gpio_set_value(23, 1);
            gpio_set_value(11, 0);
            gpio_set_value(13, 0);
            gpio_set_value(19, 0);
            gpio_set_value(21, 0);
            gpio_set_value(29, 0);
            break;
        case 2:
            gpio_set_value(11, 1);
            gpio_set_value(15, 1);
            gpio_set_value(19, 1);
            gpio_set_value(21, 1);
            gpio_set_value(29, 1);
            gpio_set_value(13, 0);
            gpio_set_value(23, 0);
            break;
        case 3:
            gpio_set_value(11, 1);
            gpio_set_value(15, 1);
            gpio_set_value(19, 1);
            gpio_set_value(23, 1);
            gpio_set_value(29, 1);
            gpio_set_value(13, 0);
            gpio_set_value(21, 0);
            break;
        case 4:
            gpio_set_value(13, 1);
            gpio_set_value(15, 1);
            gpio_set_value(21, 1);
            gpio_set_value(23, 1);
            gpio_set_value(11, 0);
            gpio_set_value(21, 0);
            gpio_set_value(29, 0);
            break;
        case 5:
            gpio_set_value(11, 1);
            gpio_set_value(13, 1);
            gpio_set_value(19, 1);
            gpio_set_value(23, 1);
            gpio_set_value(29, 1);
            gpio_set_value(15, 0);
            gpio_set_value(21, 0);
            break;
        case 6:
            gpio_set_value(11, 1);
            gpio_set_value(13, 1);
            gpio_set_value(19, 1);
            gpio_set_value(21, 1);
            gpio_set_value(23, 1);
            gpio_set_value(29, 1);
            gpio_set_value(15, 0);
            break;
        case 7:
            gpio_set_value(11, 1);
            gpio_set_value(15, 1);
            gpio_set_value(23, 1);
            gpio_set_value(13, 0);
            gpio_set_value(19, 0);
            gpio_set_value(21, 0);
            gpio_set_value(29, 0);
            break;
        case 8:
            gpio_set_value(11, 1);
            gpio_set_value(13, 1);
            gpio_set_value(15, 1);
            gpio_set_value(19, 1);
            gpio_set_value(21, 1);
            gpio_set_value(23, 1);
            gpio_set_value(29, 1);
            break;
        case 9:
            gpio_set_value(11, 1);
            gpio_set_value(13, 1);
            gpio_set_value(15, 1);
            gpio_set_value(19, 1);
            gpio_set_value(23, 1);
            gpio_set_value(29, 1);
            gpio_set_value(21, 0);
            break;
        default:
            gpio_set_value(11, 1);
            gpio_set_value(13, 1);
            gpio_set_value(15, 1);
            gpio_set_value(19, 1);
            gpio_set_value(21, 1);
            gpio_set_value(23, 1);
            gpio_set_value(29, 1);
            break;
    }
    return 0;
}

static ssize_t show_value(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    return sprintf(buf, "%i\n", default_val);
}

static ssize_t store_value(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    if (strlen(buf) == 2) {
        default_val = buf[0] - '0';
        set_default_value(default_val);
        return count;
    }
    else
        return 1;
}

static struct kobj_attribute mode_attr = __ATTR(default_val, 0660, show_value, store_value);

static struct attribute *dev_attrs[] = {
        &mode_attr.attr,
        NULL,
};

static struct attribute_group attr_group = {
        .name  = "display",
        .attrs = dev_attrs,
};

static struct kobject *dev_kobj;


static int __init dev_init(void){
    int result = 0;
    int i = 0;
    while (i< sizeof(pin_list)/sizeof(pin_list[0])){
        gpio_request(pin_list[i], "sysfs");
        gpio_direction_output(pin_list[i], true);
        gpio_set_value(pin_list[i], 1);
        gpio_export(pin_list[i], false);
        i++;
    }
    dev_kobj = kobject_create_and_add("display", kernel_kobj->parent);
    if(!dev_kobj){
        printk(KERN_ALERT "Failed to create kobject\n");
        return -ENOMEM;
    }
    result = sysfs_create_group(dev_kobj, &attr_group);
    if(result) {
        printk(KERN_ALERT "Failed to create sysfs group\n");
        kobject_put(dev_kobj);
        return result;
    }
    set_default_value(default_val);
    return 0;
}


static void __exit dev_exit(void){
    int i = 0;
    while(i<sizeof(pin_list)/sizeof(pin_list[0])){
        gpio_set_value(pin_list[i], 0);
        i++;
    }
    kobject_put(dev_kobj);
    printk(KERN_INFO "Successfully finished\n");
}


module_init(dev_init);
module_exit(dev_exit);
