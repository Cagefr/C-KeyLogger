#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/keyboard.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/ctype.h>

#define PROC_FILE_NAME "kbd_demo"

struct notifier_block nb;
const char keyMap[] = {

		' ', ' ', '1','2','3','4','5','6','7','8','9','0','-','=',' ',
		' ','q','w','e','r','t','y','u','i','o','p','[',']',' ',' ',
		'a','s','d','f','g','h','j','k','l',';',' ',' ', ' ','/','z',
		'x','c','v','b','n','m',',','.','/',' '
};
const char shiftMap[] = {

		' ',' ','!','@','#','$','%','^','&','*','(',')','_','+',' ',
		' ','Q','W','E','R','T','Y','U','I','O','P','{','}',' ',' ',
		'A','S','D','F','G','H','J','K','L',':',' ',' ', ' ','?', 'Z',
		'X','C','V','B','N','M','<','>','|',' '
};


unsigned char* list;
unsigned int length;
unsigned int hasCap;
unsigned int hasLow;
unsigned int hasNum;
unsigned int hasSym;
unsigned int pos = 0;
unsigned int verify = 0;

ssize_t read_simple(struct file *filp,char *buf,size_t count,loff_t *offp ) 
{
	return 0;
}

struct file_operations proc_fops = {
	read: read_simple,
};

int kb_notifier_fn(struct notifier_block *nb, unsigned long action, void* data){
	struct keyboard_notifier_param *kp = (struct keyboard_notifier_param*)data;
	//printk("Key:  %d  Lights:  %d  Shiftmax:  %x\n", kp->value, kp->ledstate, kp->shift);

	if (kp->value > 100)
		return 0;

	if (kp->down && kp->value != 42)
	{
		printk("Key: %c\n", keyMap[kp->value]);
		list[pos] = keyMap[kp->value];
		pos = pos + 1;
	
		if(kp->down && kp->shift == 1)
		{
			printk("CAP Key: %c\n", shiftMap[kp->value]);
			list[pos] = shiftMap[kp->value];
			pos = pos + 1;
		}
	}
	//isupper   islower  isdigit  isgraph
	if (isupper(list[pos-1])){
		hasCap = 1;
	}
	else if(islower(list[pos-1])){
		hasLow = 1;
	}
	else if(isdigit(list[pos-1])){
		hasNum = 1;
	}
	else if(isgraph(list[pos-1])){
		hasSym = 1;
	}

	verify = hasCap + hasLow + hasNum + hasSym;

	if (verify > 2)
	{
		printk("Password is valid");
	}


	return 0;
}

int init (void) {
	nb.notifier_call = kb_notifier_fn;
	register_keyboard_notifier(&nb);
	// proc_create(PROC_FILE_NAME,0,NULL,&proc_fops);

	list = kmalloc(15, GFP_KERNEL);
	return 0;
}

void cleanup(void) {
	unregister_keyboard_notifier(&nb);
	kfree(list);
	// remove_proc_entry(PROC_FILE_NAME,NULL);
}

MODULE_LICENSE("GPL"); 
module_init(init);
module_exit(cleanup);
