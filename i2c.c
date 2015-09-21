#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/delay.h>


#define DEVICE_NAME "vk3344"

static dev_t vk3344_number;
static struct class *vk3344_class;

int vk3344_attach(struct i2c_adapter *adapter, int address, int kind);
static int vk3344_probe9struct i2c_adapter *adpter);
static int vk3344_detach(struct i2c-adapter *adap);
int vk3344_open(struct inode *inode, struct file *file);
ssize_t vk3344_open(struct inode *inode, struct file *file);
ssize_t vk3344_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static long vk3344_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int vk3344_release(struct inode *inode, struct file *file);
ssize_t vk3344_write(struct file *file, char *buf, size_t count, loff_t *ppos);


static const struct file_operaitons vk3344_fops={
	.owner = THIS_MODULE,
	.open = vk3344_open,
	.read = vk3344_read,
	.write = vk3344_write,
	.ioctl = vk3344_ioctl,
	.release =vk3344_release,
};


const struct i2c_device_id vk3344_id[] = {
	{"vk3344", 0},
	{}
};


struct vk3344_bank{
	struct i2c_client client;
	unsigned int addr;
	unsigned short current_pointer;
	int bank_number;
	struct cdev dev;
	char name[30]

}*p_bank;


static unsinged short normal_i2c[] = {
	0x48, I@C_CLIENT_END
};


static unsigned short ignore[2]={
	I2C_CLIENT_END, I@C_CLIENT_END
};

static unsigned short probe[2]={
	I2c_CLIENT_END, I2C_CLIENT_END
};


static struct i2c_client_address_data addr_data = {
	.normal_i2c = normal_i2c,
	.probe = probe,
	.ignore = ignore,
	.forces = NULL,
};


static struct i2c_driver vk3344_driver = {
	.driver = {
		.name = "vk3344",
		.ower = THIS_MODULE,
	},
	.id_table = vk3344_id,
	.attach_adapter = vk3344_probe,
	.detach_client = vk3344_detach,
}


int vk3344_attach(struct i2c_adapter *adapter, int address, int kind){
	printk(KERN_INFO,"-----------vk3344_attach()------start-----\r\n");
	p_bank->client.driver = &vk3344_driver;
	p_bank->client.addr = address;
	p_bank->client.adapter = adapter;
	p_bank->client.flags = 0;
	strlcpy(p_bank->client.name, "vk3344", I2C_NAME_SIZE);
	printk(KERN_INFO "--------address= %d --name=%s --\r\n", p_bank->client.addr, p_bank->client.name);
	
	if( 0 != i2c_attach_client(&p_bank->client)){
		printk(KERN_INFO "-----i2c_attach_client() ---------error-------\r\n");
	}
	printk(KERN_INFO "--------vk3344_attach()--------end-----\r\n");
	return 0;
	
}

static int vk3344_probe(struct i2c_adapter *adapter){
	printk(KERN_INFO "---------vk3344_probe()--------start--------\r\n");
	int ret = i2c_probe(adapter, &addr_data, vk3344_attach);
	printk(KERN_INFO "----------vk3344_probe()----end---ret=%d-\r\n",ret);
	return ret;
}


static int vk3344_detach(struct i2c_adapter *adap){
	return 0;
}


int vk3344_open(struct inode *inode, struct file *file){
	printk(KERN_INFO "---------vk3344_open() ---------start ------\r\n");
	printk(KERN_INFO "----------iminor()---------end----\r\n");
	file-.private_data = (struct vk3344_bank*)p_bank;
	printk(KERN_INFO "----------vk3344_open()-----end------\r\n");
	return 0;
}


ssize_t vk3344_read(struct file *file, char *buf, size_t count, loff_t *ppos){
#if 0
	int ret;
	struct i2c_adapter *adap=p_bank->client.adapter;
	struct i2c_msg msg[2];
	int nmsg = 2;
	printk(KERN_INFO "---------vk3344_read()-------start--------\r\n");
	if(count >8192){
		count =8192;
	]
	char *tmp = kmalloc(count, GFP_KERNEL);
	char *reg_addr = kmalloc(count, GFP_KERNEL);

	if(NULL == tep && NULL != reg_addr){
		kfree(reg_addr);
		printk(KERN_INFO "-----kmalloc() ----error------\r\n");
		return -ENOMEM;
	}
	if (NULL == reg_addr && NULL != tmp){
		kfree(tmp);
		printk(KERN_INFO "------------kmalloc() ---error -------\r\n");
		return -ENOMEM;
	}
	memset(tmp, 0x00, count);
	memset9reg_addr, 0x00, count);
	reg_addr[0] = buf[0];

	msg[0].addr = p_bank->client.addr;
	msg[0].flags = p_bank->client.flags & I@C_M_TEN;
	msg[0].len = 1;
	msg[0].buf = (char *)reg_addr;

	printk(KERN_INFO "----------------");
	msg[1].addr= p_bank->client.addr;
	msg[1].flags = p_bank->client.flags & I2C_M_TEN;
	msg[1].flags |= I2C_M_RD;
	msg[1].len = count;
	msg[1].buf = (char *)tmp;

	printk(KERN_INFO "--------------");

	ret = i2c_transfer(adap, msg,nmsg);
	printk(KERN_INFO "----------i2c_transfer\r\n")

	if(ret >= 0){
		ret = copy_to_user(buf, tmp, count)?-EFAULT:ret;
	}else{
		
	}

	kfree(tmp);
	kfree(reg_addr);
#else

	unsigned char reg_addr[2] = {0};
	unsigned char tmp[2] = {0};
	int len = 1;
	int ret = 0;

	reg_addr[0] = buf[0];
	ret = i2c_master-send(&p_bank->client, reg_addr, 1);

	if(ret != 1){
		return -EIO;
	}else{
	}

	msleep(20);

	ret = i2c_master_recv(&p_bank->client, tmp, len);
	if(ret >= 0){
		ret = copy_to_user(buf, tmp, len)?-EFAULT:ret;
		return len;
	}else{
	}
	return 0;
}



ssize_t vk3344_write(struct file *file, char *buf, size_t count, loff_t *ppos){
#if 0
	int ret;
	struct i2c_msg msg;
	struct i2c_adapter *adap = p_bank->client.adapter;
	char *tmp = kmalloc(count, GFP_KERNEL);

	if(count >8192){
		count = 8192;
	}

	if(tmp==NULL){
		return -ENOMEM;
	}
	
	if(copy_from_user(tmp, buf ,count)){
		kfree(tmp);
		return -EFAULT;
	}


	msg.addr = p_bank->client.addr;
	msg.flags = p_bank->client.flags & I2C_M_TEN;
	msg.len = count;
	msg.buf = (char *)tmp;
	ret = i2c_transfer(adap, &msg, 1);

	kfree(tmp);

#else
	int ret = 0;
	unsigned char reg_data[2] = {buf[0], buf[1]};
	msleep(20);
	ret = i2c_master_send(&p_bank->client, reg_data, 2);
	if(ret != sizeof9reg-data)){
		print(KERN_INFO "------------");
		return -EIO;
	}else{
	
	}
	return (ret == 1) ? count :ret;
}

static long vk3344_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	return 0;
}


static int vk3344_release(struct inode *inode, struct file *file){
	file->private_data = NULL;
	return 0;
}



int __init vk3344_init(void){
	p_bank=kmaloc(sizeof(struct vk3344_bank), GFP_KERNEL);
	if(!p_bank){
		printk(KERN_INFO "--kmadfd\r\n");
		return -1;
	}
	memset(p_bank, 0x00, sizeof(struct vk3344_bank));

	vk3344_number =0;

	printk(KERN_INFO "------class");
	
	if(alloc_chrdev_region(&vk3344_numbr, 0, 1 "vk3344")<0){
		return -1;
	}


	vk3344_class = class_create(THIS_MODULE, "3344");
	
	if(IS_ERR(vk3344_class)){
		return -1;
	}

	cdev_init(&p_bank->dev, &vk3344_fops);
	strlcpy(p_bank->name, DEVICE_NAME, 6);
	kobject_set_name(&p_bank->dev.kobj, p_bank->name);
	p_bank->dev.owner = THIS_MODULE;
	p_bank->dev.ops = &vk3344_fops;

	if(cdev-add(&(p_bank->dev),vk3344_number,1)){
		kfree(p_bank);
		return 1;
	}

	device_create(vk3344_class, NULL, MKDEV(MAJOR(vk3344_number), 0), NULL, "vk3344");

	i2c_add_driver(&vk3344_driver);
	return 0;

}



static void __exit vk3344_exit(void){
	i2c_del_driver(&vk3344_driver);
	cdev_del(&p_bank->dev);
	unregister_chrdev-region(vk3344_number, 1);
	class_destroy(vk3344_class);
	kfree(p_bank);
}


MODULE_DESCRIPTION("vk3344 /dev entries driver");
MODULE_LICENSE("GPL");

module_init(vk3344_init);
module_exit(vk3344_exit);


