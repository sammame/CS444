/*
 * A sample, extra-simple block driver. Updated for kernel 2.6.31.
 *
 * (C) 2003 Eklektix, Inc.
 * (C) 2010 Pat Patterson <pat at superpat dot com>
 * Redistributable under the terms of the GNU GPL.
 */


#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>

#include <linux/crypto.h> //default linux crypto libarary 
			  //used for encryption and decrption schemes
 
MODULE_LICENSE("Dual BSD/GPL");
//static char *Version = "1.4";

/* Crypto cipher struct (tfm = transformation) */
static struct crypto_cipher *tfm;

static int major_num = 0;
module_param(major_num, int, 0);
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);
static int nsectors = 1024; /* How big the drive is */
module_param(nsectors, int, 0);


/* Encryption key */
static char* key = "42_Fall2017";
module_param(key, charp, 0);

/* Sector Size */
#define KERNEL_SECTOR_SIZE 512

/*
 * Request queue.
 */
static struct request_queue *Queue;

/*
 * Device struct representation.
 */
static struct sbd_device {
        unsigned long size;
        spinlock_t lock; /* locks device. */
        u8 *data; 
        struct gendisk *gd; 
} Device;

/*
 * Handle an I/O request.
 *
 * The bulk of the changes we made form the origional source are in this function.
 * Added I/O handling for read and write.
 * Include encryption and decryption schemes 
*/
static void sbd_transfer(struct sbd_device *dev, sector_t sector,
	unsigned long nsect, char *buffer, int write)
{
  unsigned long offset = sector * logical_block_size;
  unsigned long numbytes = nsect * logical_block_size;

  /* Many of the changes start here */ 
  u8 *hex_str;
  u8 *hex_buf;
  u8 *hex_disk;
  int i;

  /* Create buffer for encryption and decryption */
  hex_disk = dev->data + offset;
  hex_buf = buffer;

  /* Prevent from writing beyond file size  */
  if ((offset + numbytes) > dev->size)
  {
	printk(KERN_NOTICE "Error: File Overflow (%ld, %ld)\n", offset, numbytes);
	return;
  }

  /* Encrypt and write */
  if (write)
  {
	printk("\n\nBegin Encryption\n");

	/* encrypts only one block at a time to new data size */
	for(i = 0; i < numbytes; i += crypto_cipher_blocksize(tfm))
	{
		crypto_cipher_encrypt_one(tfm, hex_disk + i, hex_buf + i);
	}
		
	hex_str = buffer;
	printk("Data before encryption: ");
	for(i = 0; i < 15; i++)
	{
		printk("%02x", hex_str[i]);
	}

	hex_str = dev->data + offset;
	hex_str = dev->data + offset;
	printk("\nData after encryption: ");
	for(i = 0; i < 15; i++)
	{
		printk("%02x", hex_str[i]);
	}
	printk("\nEncryption Successful\n");
  } else {
    	printk("\n\nBegin Decryption\n");

        for(i = 0; i < numbytes; i += crypto_cipher_blocksize(tfm))
   	{
		crypto_cipher_decrypt_one(tfm, hex_buf + i,hex_disk + i);
        }

        hex_str = dev->data + offset;
	printk("Data before decryption: ");
	for(i = 0; i < 15; i++)
	{
		printk("%02x", hex_str[i]);
	}

        hex_str = buffer;
	printk("\nData after decryption: ");
	for(i = 0; i < 15; i++)
	{
		printk("%02x", hex_str[i]);
	}
        printk("\nDecryption Successful\n");
  }
}

/* No real changes from source. Added hotfix for new version */
static void sbd_request(struct request_queue *q) {
	struct request *req;

	req = blk_fetch_request(q);
	while (req != NULL) {
		if (req == NULL || (req->cmd_type != REQ_TYPE_FS)) {
			printk(KERN_NOTICE "Skip non-CMD request\n");
			__blk_end_request_all(req, -EIO);
			continue;
		}

		 // Fix for new version found in comment. Change req->buffer to bio_data(req-bio)
		sbd_transfer(&Device, blk_rq_pos(req), blk_rq_cur_sectors(req),
			bio_data(req->bio), rq_data_dir(req));

		if ( ! __blk_end_request_cur(req, 0) ) {
			req = blk_fetch_request(q);
		}
	}
}

/*
 * The HDIO_GETGEO ioctl is handled in blkdev_ioctl(), which
 * calls this. We need to implement getgeo, since we can't
 * use tools such as fdisk to partition the drive otherwise.
 *
 * Nothing was changed in this section
 */
int sbd_getgeo(struct block_device * block_device, struct hd_geometry * geo) {
	long size;

	/* We have no real geometry, of course, so make something up. */
	size = Device.size * (logical_block_size / KERNEL_SECTOR_SIZE);
	geo->cylinders = (size & ~0x3f) >> 6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 0;
	return 0;
}

/*
 * The device operations structure.
 *
 * No changes here for hw3 
 */
static struct block_device_operations sbd_ops = {
		.owner  = THIS_MODULE,
		.getgeo = sbd_getgeo
};


/* Minor changes added to include initialization of the crypto cipher */
static int __init sbd_init(void) {

	printk("\n\nHello from sbd_init!\n\n");

	/*
	 * Set up our internal device.
	 */
	Device.size = nsectors * logical_block_size;
	spin_lock_init(&Device.lock);
	Device.data = vmalloc(Device.size);
	if (Device.data == NULL)
		return -ENOMEM;
	/*
	 * Get a request queue.
	 */
	Queue = blk_init_queue(sbd_request, &Device.lock);
	if (Queue == NULL)
		goto out;
	blk_queue_logical_block_size(Queue, logical_block_size);
	/*
	 * Get registered.
	 */
	major_num = register_blkdev(major_num, "sbd");
	if (major_num < 0) {
		printk(KERN_WARNING "Error: Registered Block Device Failed\n");
		goto out;
	}

	/* allocing crypto cipher */
	tfm = crypto_alloc_cipher("aes",0,0);
	if(!tfm){
		printk(KERN_WARNING "Error: Cipher Allocation Failed\n");
		goto out;
	}

	crypto_cipher_setkey(tfm,key,strlen(key));

	/*
	 * And the gendisk structure.
	 */
	Device.gd = alloc_disk(16);
	if (!Device.gd)
		goto out_unregister;
	Device.gd->major = major_num;
	Device.gd->first_minor = 0;
	Device.gd->fops = &sbd_ops;
	Device.gd->private_data = &Device;
	strcpy(Device.gd->disk_name, "sbd0");
	set_capacity(Device.gd, nsectors);
	Device.gd->queue = Queue;
	add_disk(Device.gd);

	return 0;

out_unregister:
	unregister_blkdev(major_num, "sbd");
out:
	vfree(Device.data);
	return -ENOMEM;
}



// Clean up crypto memory
static void __exit sbd_exit(void)
{
	printk("\n\nGoodbye from sbd_exit!\n\n");

	/* Freeing crypto */
	crypto_free_cipher(tfm);

	del_gendisk(Device.gd);
	put_disk(Device.gd);
	unregister_blkdev(major_num, "sbd");
	blk_cleanup_queue(Queue);
}

module_init(sbd_init);
module_exit(sbd_exit);
