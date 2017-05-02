/*
 * "Hello, world!" minimal kernel module - /dev version
 *
 * Valerie Henson <val@nmt.edu>
 *
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>
#include <asm/uaccess.h>

/*
 * hello_read is the function called when a process calls read() on
 * /dev/hello.  It writes "Hello, world!" to the buffer passed in the
 * read() call.
 */
 //get random bytes, make 0-5, and copy_to_user.
int copy(int len, char * buf){
	unsigned char mybuf[32];
	int i;
while(len != 0){
	get_random_bytes(mybuf, sizeof(mybuf));
	for(i = 0; i < sizeof(mybuf); i++){
		mybuf[i] = mybuf[i] % 6;
	}
	if (len <= 32){
		if (copy_to_user(buf, mybuf, len)){
			return -EINVAL;
		}
		buf = buf + len;
		len = 0;
	}else{
		if (copy_to_user(buf, mybuf, 32)){
			return -EINVAL;
		}
		buf = buf + 32;
		len = len - 32;
	}
	i = 0;
}
	return len;
}

static ssize_t dice_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	//keep track of the number of bytes we still need to write
	//unsigned char mybuf[32];
	int len = count;
	//int i;
	//write count bytes to user
	len = copy(len, buf);
	printk("done\n");
	//Tell the user how much data we wrote.
	*ppos = *ppos + count;
	return count;
}

/*
 * The only file operation we care about is read.
 */

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
};

static struct miscdevice dice_dev = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/hello.
	 */
	"dice",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&dice_fops
};

static int __init
dice_init(void)
{
	int ret;

	/*
	 * Create the "hello" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/hello device using
	 * the default rules.
	 */
	ret = misc_register(&dice_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register \"Dice, world!\" misc device\n");

	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void)
{
	misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Good <klg92@pitt.edu>");
MODULE_DESCRIPTION("\"Dice, world!\" minimal module");
MODULE_VERSION("dev");
