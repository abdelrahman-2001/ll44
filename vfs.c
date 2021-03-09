#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");


struct myfile{
	struct file *f;
	mm_segment_t fs;
	loff_t pos;
};

//--------------------------------------------------------------------------------------

struct myfile *open_file_for_read(char *filename){
        
	struct myfile *fi;
	fi = kmalloc(sizeof(struct myfile) , GFP_KERNEL);
	fi->fs = get_fs();
	set_fs(get_ds());
	fi->f = filp_open(filename , O_RDWR , 0);
 	set_fs(fi->fs);

	if (IS_ERR(fi->f)){
		printk(KERN_INFO "File failed to open\n");
		kfree(fi);
		return NULL;
	}else printk(KERN_INFO "File opened\n");
	return fi;

}

//--------------------------------------------------------------------------------------

volatile int read_from_file_until(struct myfile *mf,char *buf, unsigned long vlen, char c){

	int sss;
	
	mf->fs = get_fs();
	set_fs(get_ds());
	mf->pos = 0;
      
        sss = vfs_read(mf->f , buf , vlen , &(mf->pos));	
        set_fs(mf->fs);

	printk(KERN_INFO "%s \n" , buf);	

	return sss;
	
}

//--------------------------------------------------------------------------------------

void close_file(struct myfile * mf){
	if(mf->f){
		filp_close(mf->f , NULL);	//to clode the file
		printk(KERN_INFO "File closed");	
	}
}


//-------------------------------------------------------------------------------------

int init(void){
	struct myfile *ff;	//struct to hold the opened file
	char *buf;		//buffer for read function
	int ss;			//returned back from read function
	printk(KERN_INFO "init function\n");

	ff = kmalloc(sizeof(struct myfile) , GFP_KERNEL);	//allocating memory space for struct
	ff = open_file_for_read("../../proc/version");	//call open function and savein ff	

	buf = kmalloc(sizeof(char) , GFP_KERNEL);	//allocate memory space
	ss = read_from_file_until(ff, buf, 31, ' ');		//read and print buf
	
	close_file(ff);				//close the file
	
	kfree(ff);		//free memory
	kfree(buf);
	return 0;
}

void cleanup(void){
	printk(KERN_INFO "cleanup function\n");
}

module_init(init);
module_exit(cleanup);
