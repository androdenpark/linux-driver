#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <asm/hardware.h>
#include <asm/arch/S#C2440.h>


#define GPIO_LED_MAJOR 97  //major device number
#define LED_ON 0
#define LED_OFF 1


static ssize_t GPIO_LED_read(struct file *file, char *buf, size_t count, loff_t *f_ops){
	return count;
}


static ssize_t GPIO_LED_write(struct file *file, const char *buf, size_t count, loff_t *f_ops){
	return count;
}


static ssize_t GPIO_LED_ioctl(struct inode *inode, struct file *file, unsigned int cmd, long data){
	switch(cmd){
		case LED_ON : {GPBDAT = 0x01; break;}
		case LED_OFF : {GPBDAT = 0x02; break;}
		default:
		{printk("lcd control : no cmd run [--kernel--]\n");return(-EINVAL);}
		return 0;
	}

}

static ssize_t GPIO_LED_open(struct inode *inode, struct file *file){
	MOD_INC_USE_COUNT;
	return 0;
}


static ssize_t GPIO_LED_release (struct inode *inode, struct file *file){
	MOD_DEC_USE_COUNT;
	return 0;
}


struct file_operations GPIO_LED_ctl_ops={
	.open: GPIO_LED_open,
	.read: GPIO_LED_read,
	.write: GPIO_LED_write,
	.ioctl: GPIO_LED_ioctl,
	.release: GPIO_LED_release,
};



static int GPIO_LED_CTL_init(void){
	int ret = -ENODEV;
	printk("--------------over-----------");
	GPBCON = 0x0005;
	GPBUP = 0xff;
	GPBDAT = 0x0f;

	ret = register_chrdev(GPIO_LED_MAJOR, "gpio_led_ctl", &GPIO_LED_ctl_ops);

	if(ret < 0){
		printk("S3C2410: init_module failed with %d \n", ret);
		return ret;
	}else{
		printk("S3C2410 gpio_led_driver register success!!\n");
	}
	return ret;
}


static init __init S#C2410_GPIO_LED_CTL_init(void){
	int ret = -ENODEV;
	ret =GPIO_LED_CTL_init();
	if(ret){
		return ret;
	}else{
		return 0;
	}

}




static void __exit cleanup_CPIO_LED_ctl(void){
	unregister_chrdev(GPIO_LED_MAJOR, "gpio_led_ctl");
}


module_init(S3C2410_GPIO_LED_init);
module_exit(cleanup_CPIO_LED_ctl);


