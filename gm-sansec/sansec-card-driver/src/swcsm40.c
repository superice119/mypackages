//#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,12,1)
	#include <asm/uaccess.h>
#else
	#include <linux/uaccess.h>
#endif 

#define _SWXA_POLLING_
//#define _KERNEL_CALLING_

#define VENDOR_ID	0x10EE
#define DEVICE_ID	0x7024
#define DEVICE_NEW_ID	0x7021

#define MAX_BUFFER_LEN 65536
#define MAX_DATA_BUFFER_LEN	MAX_BUFFER_LEN-256

#define H2C_DMA_ID                0x0000
#define H2C_DMA_ST                0x0004
#define H2C_DMA_STA               0x0040
#define H2C_DMA_DESC              0x0090

#define C2H_DMA_ID                0x1000
#define C2H_DMA_ST                0x1004
#define C2H_DMA_STA               0x1040
#define C2H_DMA_DESC              0x1090

#define IRQ_DMA_EN               0x2004
#define IRQ_DMA_DISEN            0x200C
#define IRQ_DMA_CHNNEL           0x2010
#define IRQ_DMA_DISCHNNEL        0x2018
#define IRQ_DESC_VECTOR          0x2080

#define H2C_DESC_ID               0x4000
#define H2C_DESC_LOW_ADDR         0x4080
#define H2C_DESC_HIGH_ADDR        0x4084
#define H2C_DESC_NXT_CTRL         0x4088

#define C2H_DESC_ID               0x5000
#define C2H_DESC_LOW_ADDR         0x5080
#define C2H_DESC_HIGH_ADDR        0x5084
#define C2H_DESC_NXT_CTRL         0x5088

#if defined IRQF_SHARED
	#define SA_SHIRQ IRQF_SHARED
#endif

#define SWXA_RESET_COMMAND  _IO(0x0F, 01)
#define SWXA_GET_SUBDEVID   _IO(0x0F, 02)
#define IOCTL_OPERATION_CODE   _IO(0x0F, 03)
#define	SWXA_MAX_DEV_NUM	4
#define	ECCref_MAX_LEN	68
#define	OPT_ECC_INIT	0x00000511

static int  swxa_major = 113;  // 0  113
static char Swxa_Dev_Stat[SWXA_MAX_DEV_NUM];

struct SWXA_DEV {
	unsigned long  swxa_iobase0;
	unsigned long  swxa_io0Len;
	unsigned long  swxa_membase0;
	
	unsigned int *swxa_dma_buffer;
	unsigned int *swxa_dma_bufferout;	
	unsigned int *swxa_dma_bufferKelIn;
	unsigned int *swxa_dma_bufferKelOut;	
	unsigned int *swxa_des_buffer;
	unsigned int *swxa_des_bufferout;
	
	unsigned int  swxa_readbytesIOCTL;	
	unsigned int  swxa_readintIOCTL;
	unsigned int  swxa_IRQ_NUM;
	 
	spinlock_t swxa_int_lock;
	spinlock_t swxa_rw_lock;
	struct mutex swxa_wr_mutex;
	
	wait_queue_head_t swxa_wait_on_interrupt;
	int  swxa_wakeup_flag;

	struct pci_dev *pdev;
	struct cdev cdev;	  /* Char device structure		*/
	
	unsigned int  SubDevID;		
	short type;
	dma_addr_t pPhysicalInAddr;
	dma_addr_t pPhysicalOutAddr;
	dma_addr_t pPhysicalInAddrKel;
	dma_addr_t pPhysicalOutAddrKel;	
	dma_addr_t pPhysicalDesInAddr;
	dma_addr_t pPhysicalDesOutAddr;
	
};
struct class *swxa_class;

typedef struct ECCrefCurveParam_st
{ 
  unsigned char p[ECCref_MAX_LEN];  //素数p
  unsigned char a[ECCref_MAX_LEN];  //参数a
  unsigned char b[ECCref_MAX_LEN];  //参数b
  unsigned char gx[ECCref_MAX_LEN];  //参数Gx: x coordinate of the base point G
  unsigned char gy[ECCref_MAX_LEN];  //参数Gy: y coordinate of the base point G
  unsigned char n[ECCref_MAX_LEN];  //阶N: order n of the base point G
  unsigned int  len;          //参数位长Len，Len必须为160、192、224或256
} ECCrefCurveParam;

typedef struct SM3_CONTEXT_st
{
	unsigned int stateIV[8];	/*state (ABCDEFGH)*/
	unsigned int count[2];	/*number of bits, modulo 2^64 (lsb first) */
	unsigned char  buffer[64];	/* input buffer */
} SM3_CONTEXT;

typedef struct ECCrefPublicKey_st
{
	unsigned int  bits;
	unsigned char x[32]; 
	unsigned char y[32]; 
} ECCrefPublicKey;

static struct pci_device_id swxa_ids[] = {
	{PCI_DEVICE(VENDOR_ID, DEVICE_ID),},
	{PCI_DEVICE(VENDOR_ID, DEVICE_NEW_ID),},
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, swxa_ids);

static void swxa_Dialog(struct SWXA_DEV *swxa_dev, int nWrNum, int nRdNum);
int ConvertIntEndianCode(unsigned char *pbDestBuffer, unsigned char *pbSrcBuffer, unsigned int unBuferLength)
{
	unsigned int i;

	if(unBuferLength & 0x3)
		return -1;

	for(i=0; i < unBuferLength; )
	{
		*pbDestBuffer++ = *(pbSrcBuffer + i + 3);
		*pbDestBuffer++ = *(pbSrcBuffer + i + 2);
		*pbDestBuffer++ = *(pbSrcBuffer + i + 1);
		*pbDestBuffer++ = *(pbSrcBuffer + i);
		i+= 4;
	}

	return 0;
}

int t_ECCInit(struct SWXA_DEV *swxa_dev)
{
	int swxa_readbytes;
	ECCrefCurveParam  Ecc_Curve;
	unsigned char ECC_256[196]={0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
								0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc,
								0x5a, 0xc6, 0x35, 0xd8, 0xaa, 0x3a, 0x93, 0xe7,0xb3, 0xeb, 0xbd, 0x55, 0x76, 0x98, 0x86, 0xbc,0x65, 0x1d, 0x06, 0xb0, 0xcc, 0x53, 0xb0, 0xf6,0x3b, 0xce, 0x3c, 0x3e, 0x27, 0xd2, 0x60, 0x4b,
								0x6b, 0x17, 0xd1, 0xf2, 0xe1, 0x2c, 0x42, 0x47,0xf8, 0xbc, 0xe6, 0xe5, 0x63, 0xa4, 0x40, 0xf2,0x77, 0x03, 0x7d, 0x81, 0x2d, 0xeb, 0x33, 0xa0,0xf4, 0xa1, 0x39, 0x45, 0xd8, 0x98, 0xc2, 0x96,
								0x4f, 0xe3, 0x42, 0xe2, 0xfe, 0x1a, 0x7f, 0x9b,0x8e, 0xe7, 0xeb, 0x4a, 0x7c, 0x0f, 0x9e, 0x16,0x2b, 0xce, 0x33, 0x57, 0x6b, 0x31, 0x5e, 0xce,0xcb, 0xb6, 0x40, 0x68, 0x37, 0xbf, 0x51, 0xf5,
								0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xbc, 0xe6, 0xfa,0xad,  0xa7, 0x17, 0x9e, 0x84, 0xf3, 0xb9,0xca, 0xc2,0xfc,  0x63,0x25,  0x51,
								0
								};
	
	unsigned int Descriptor_H2C[8]  = {0xAD4B0002, 0x00002000, (unsigned int)swxa_dev->pPhysicalInAddr, 0x00000000,
								 	   0x40000000, 0x00000000, 0x40000000, 0x00000000};
 	unsigned int Descriptor_C2H[8]  = {0xAD4B0003, 0x00002000, 0x40000000, 0x00000000,
								(unsigned int)swxa_dev->pPhysicalOutAddr, 0x00000000, 0x40000000, 0x00000000};								
	memcpy(swxa_dev->swxa_des_buffer, Descriptor_H2C, 32);
	memcpy(swxa_dev->swxa_des_bufferout, Descriptor_C2H, 32);
	
	memset(&Ecc_Curve,0x0,sizeof(ECCrefCurveParam));
	memcpy(Ecc_Curve.p+ECCref_MAX_LEN-32,ECC_256,32);
	memcpy(Ecc_Curve.a+ECCref_MAX_LEN-32,ECC_256+32,32);
	memcpy(Ecc_Curve.b+ECCref_MAX_LEN-32,ECC_256+64,32);
	memcpy(Ecc_Curve.gx+ECCref_MAX_LEN-32,ECC_256+96,32);
	memcpy(Ecc_Curve.gy+ECCref_MAX_LEN-32,ECC_256+128,32);
	memcpy(Ecc_Curve.n+ECCref_MAX_LEN-32,ECC_256+160,32);
	Ecc_Curve.len=256;
	
	swxa_dev->swxa_dma_buffer[0] = 4+103,//4+512+512+256;
	swxa_dev->swxa_dma_buffer[1] = 2;
	swxa_dev->swxa_dma_buffer[2] = OPT_ECC_INIT;
	swxa_dev->swxa_dma_buffer[3] = (0<<16)|0;
	ConvertIntEndianCode((unsigned char *)&swxa_dev->swxa_dma_buffer[4], (unsigned char *)&Ecc_Curve, sizeof(ECCrefCurveParam)-4);
 	swxa_dev->swxa_dma_buffer[106]=Ecc_Curve.len;
	
	swxa_readbytes = swxa_dev->swxa_dma_buffer[1]<<2;  // 读取的字节长度
    swxa_Dialog(swxa_dev, swxa_dev->swxa_dma_buffer[0]<<2, swxa_readbytes);
    if(swxa_dev->swxa_dma_bufferout[1])
    {
		printk(KERN_ERR"OPT_ECC_INIT Err! ErrCode = 0x%08x\n", swxa_dev->swxa_dma_bufferout[1]);
		return -1;
	}
	else
		printk(KERN_ERR"OPT_ECC_INIT ok!\n");
		
	return 0;
}

static void swxa_reset(struct SWXA_DEV *swxa_dev)
{
	mdelay(1000);
	//data = ioread32((void*)swxa_dev->swxa_membase0 + DMA_C2H_CTRL);
	//printk("FPGA VER:%x\n",((data>>12)&0x0000000f));
	//mdelay(1000);
	
	iowrite32(0x7ffffff,(void*)swxa_dev->swxa_membase0 + 0x000c);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + 0x0044);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + H2C_DMA_STA); //clear status
	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + 0x0098);

	iowrite32(0x7ffffff,(void*)swxa_dev->swxa_membase0 + 0x100c);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + 0x1044);
	
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + C2H_DMA_STA); //clear status
	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + 0x1098);
	
	mdelay(1000);
	iowrite32(0x01,(void*)swxa_dev->swxa_membase0 + 0x2018);
	mdelay(1000);
  	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + H2C_DMA_DESC);
  	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + C2H_DMA_DESC);
	
	mdelay(1000);

	return;
}

int swxa_open(struct inode *inode, struct file *filep)
{
	struct SWXA_DEV *swxa_dev;  /* device information */

	swxa_dev = container_of(inode->i_cdev, struct SWXA_DEV, cdev);
	filep->private_data = swxa_dev; /* for other methods */
	return 0;
}

int swxa_close(struct inode *inode, struct file *filep)
{
	return 0;
}

ssize_t swxa_write(struct file *filep, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct SWXA_DEV *swxa_dev = filep->private_data;
	int swxa_readbytes;
	unsigned int Descriptor_H2C[8]  = {0xAD4B0002, 0x00002000, (unsigned int)swxa_dev->pPhysicalInAddr, 0x00000000,
								 	   0x40000000, 0x00000000, 0x40000000, 0x00000000};								 
	unsigned int Descriptor_C2H[8]  = {0xAD4B0003, 0x00002000, 0x40000000, 0x00000000,(unsigned int)swxa_dev->pPhysicalOutAddr, 
									   0x00000000, 0x40000000, 0x00000000};								
	
	memcpy(swxa_dev->swxa_des_buffer, Descriptor_H2C, 32);
	memcpy(swxa_dev->swxa_des_bufferout, Descriptor_C2H, 32);

 	mutex_lock(&swxa_dev->swxa_wr_mutex);

	if(count>MAX_DATA_BUFFER_LEN)
		count = MAX_DATA_BUFFER_LEN;
	
	if(copy_from_user(swxa_dev->swxa_dma_buffer, buf, count)){
		mutex_unlock(&swxa_dev->swxa_wr_mutex);  
 
		return -EFAULT;
	}

	swxa_readbytes = swxa_dev->swxa_dma_buffer[1]<<2;  // 读取的字节长度
    swxa_Dialog(swxa_dev, count, swxa_readbytes);

	mutex_unlock(&swxa_dev->swxa_wr_mutex);    	

	return 0;
}

ssize_t swxa_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct SWXA_DEV *swxa_dev = filp->private_data;
	
 	mutex_lock(&swxa_dev->swxa_wr_mutex); 	

	if(copy_to_user(buf, swxa_dev->swxa_dma_bufferout, count)){
		mutex_unlock(&swxa_dev->swxa_wr_mutex);  
	 
		return -EFAULT;
	}
	
	mutex_unlock(&swxa_dev->swxa_wr_mutex);  
 
	return 0;
}

static void swxa_Dialog(struct SWXA_DEV *swxa_dev, int nWrNum, int nRdNum)
{
/*	dma_addr_t pPhysicalInAddr, pPhysicalOutAddr;
	if(mode==1){
		pPhysicalInAddr = swxa_dev->pPhysicalInAddr;
		pPhysicalOutAddr = swxa_dev->pPhysicalOutAddr;
	}
	else if(mode==2){
		pPhysicalInAddr = swxa_dev->pPhysicalInAddrKel;
		pPhysicalOutAddr = swxa_dev->pPhysicalOutAddrKel;
	}
	else
		return ;
*/

#ifndef _SWXA_POLLING_		
	iowrite32(0x01,(void*)swxa_dev->swxa_membase0 + IRQ_DMA_CHNNEL);
	iowrite32(0x00000000,(void*)swxa_dev->swxa_membase0 + 0x20A0);
	
#endif
	//iowrite32(0x00000000,(void*)swxa_dev->swxa_membase0 + 0x3040);
	// H2C - descriptor
	iowrite32(swxa_dev->pPhysicalDesInAddr,(void*)swxa_dev->swxa_membase0 + H2C_DESC_LOW_ADDR);
	iowrite32(0x00000000,(void*)swxa_dev->swxa_membase0 + H2C_DESC_HIGH_ADDR);
	iowrite32(0x00000000,(void*)swxa_dev->swxa_membase0 + H2C_DESC_NXT_CTRL);
	
	// DMA Control
	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + H2C_DMA_DESC);
	// H2C start
	iowrite32(0x07,(void*)swxa_dev->swxa_membase0 + H2C_DMA_ST);
	while(!(ioread32((void*)swxa_dev->swxa_membase0+H2C_DMA_STA) & 0x00000004))
	{;}
	
	// C2H - descriptor
	iowrite32(swxa_dev->pPhysicalDesOutAddr,(void*)swxa_dev->swxa_membase0 + C2H_DESC_LOW_ADDR);
	iowrite32(0x00000000,(void*)swxa_dev->swxa_membase0 + C2H_DESC_HIGH_ADDR);
	iowrite32(0x00000000,(void*)swxa_dev->swxa_membase0 + C2H_DESC_NXT_CTRL);
	
	// DMA Control
	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + C2H_DMA_DESC);

#ifndef _SWXA_POLLING_		
		//wait_event(swxa_dev->swxa_wait_on_interrupt, swxa_dev->swxa_wakeup_flag != 0);
		wait_event_interruptible(swxa_dev->swxa_wait_on_interrupt, swxa_dev->swxa_wakeup_flag != 0);
		swxa_dev->swxa_wakeup_flag = 0;	
#else
  	while(!(ioread32((void*)swxa_dev->swxa_membase0+C2H_DMA_STA) & 0x00000004))
		barrier();
	
	iowrite32(0x07ffffff,(void*)swxa_dev->swxa_membase0 + 0x100c);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + C2H_DMA_STA); //clear status
	
#endif

	return;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
int swxa_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#else
long swxa_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
	int err = 0;
    struct SWXA_DEV *swxa_dev;  /* device information */
    
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	swxa_dev = container_of(inode->i_cdev, struct SWXA_DEV, cdev);
#else
	swxa_dev = filp->private_data;
#endif
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	
	if (_IOC_TYPE(cmd) != 0x0F)
		return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR)
		return -ENOTTY;
	*/

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
#else
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
	if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok((void __user *)arg, _IOC_SIZE(cmd));
#endif
	if (err) 
		return -EFAULT;

	switch(cmd)
	{
		case SWXA_RESET_COMMAND:
		{
			swxa_reset(swxa_dev);
			break;
		}
		case SWXA_GET_SUBDEVID:
		{
			return swxa_dev->SubDevID;
		}
	
		default:
			return -ENOTTY;
	}
	return 0;
}


struct file_operations swxa_fops = {
	.owner	 = THIS_MODULE,
	.read	 = swxa_read,
	.write	 = swxa_write,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl   = swxa_ioctl,
#else
	.unlocked_ioctl = swxa_ioctl,
#endif
	.open	 = swxa_open,
	.release = swxa_close,
};

#ifndef _SWXA_POLLING_

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
	static irqreturn_t swxa_handler(int irq, void *dev_id, struct pt_regs *regs)  //2.6.19以前的版本
#else
	static irqreturn_t swxa_handler(int irq, void *dev_id)  //2.6.19之后的版本
#endif
{
	unsigned int irqflag;		
	struct SWXA_DEV *swxa_dev = dev_id;
	
	irqflag = ioread32((void*)swxa_dev->swxa_membase0 + C2H_DMA_STA);
	if (!(irqflag & 0x00000004))
		return IRQ_NONE;		//not our interrupt

	iowrite32(0x7ffffff,(void*)swxa_dev->swxa_membase0 + 0x000c);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + 0x0044);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + H2C_DMA_STA); //clear status
	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + 0x0098);
	
	
	iowrite32(0x7ffffff,(void*)swxa_dev->swxa_membase0 + 0x100c);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + 0x1044);
	iowrite32(0xfffffe,(void*)swxa_dev->swxa_membase0 + C2H_DMA_STA); //clear status
	iowrite32(0x06,(void*)swxa_dev->swxa_membase0 + 0x1098);
	// IRQ
	iowrite32(0x01,(void*)swxa_dev->swxa_membase0 + 0x2018);

	swxa_dev->swxa_wakeup_flag = 1;
	wake_up(&swxa_dev->swxa_wait_on_interrupt);

	return IRQ_HANDLED;
}
#endif

static int swxa_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	/* Do probing type stuff here.  
	 * Like calling request_region();
	 */
	int rc, devnum;
	struct SWXA_DEV *swxa_dev;
	char buf[20];//buffer store character device node name
		
	//unsigned short cfgid;
	/* find a free range for device files */
	for (devnum = 0; devnum < SWXA_MAX_DEV_NUM; devnum++)
	{
		if (Swxa_Dev_Stat[devnum] == 0)
		{
			Swxa_Dev_Stat[devnum] = 1;
			break;
		}
	}
	
	swxa_dev = kmalloc(sizeof(struct SWXA_DEV), GFP_KERNEL);
	if (swxa_dev == NULL)
	{
		rc = -ENOMEM;
		goto out;  /* Make this more graceful */
	}
	
	swxa_dev->pdev = dev;		
	rc = pci_enable_device(swxa_dev->pdev);
	if (rc < 0)
	{
		printk(KERN_ERR "pci_enable_device failure = 0x%x\n", rc);
		goto free_part0;
	}
	
	swxa_dev->swxa_iobase0 = pci_resource_start(dev, 0);
	swxa_dev->swxa_io0Len = pci_resource_len(dev, 0);
//	printk(KERN_ERR "1---- swxa_iobase0 = 0x%lx, Len = 0x%lx\n", swxa_dev->swxa_iobase0, swxa_dev->swxa_io0Len);
	
//	swxa_dev->swxa_iobase1 = pci_resource_start(dev, 2);
//	swxa_dev->swxa_io1Len = pci_resource_len(dev, 2);
	//printk(KERN_ERR "2---- swxa_iobase1 = 0x%lx, Len = 0x%lx\n", swxa_dev->swxa_iobase1, swxa_dev->swxa_io1Len);		

	pci_read_config_word(dev, 0x2, &swxa_dev->type);
	if(swxa_dev->type==0x7021)
		swxa_dev->type = 42;
	else 
		swxa_dev->type = 40;
	printk("type = 0x%x\n", swxa_dev->type);
	pci_read_config_dword(dev, 0x2C, &swxa_dev->SubDevID);  //获取子设备ID
	//pci_write_config_word(dev, 0x4, 0x7);// 
 	
	swxa_dev->swxa_dma_buffer = dma_alloc_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN, &swxa_dev->pPhysicalInAddr, GFP_KERNEL);
   	if (swxa_dev->swxa_dma_buffer == NULL)
	{
		printk(KERN_ERR "swxa_dev->swxa_dma_buffer kmalloc error!\n");
		rc = -ENOMEM;
		goto disable_dev;
	}
     
	swxa_dev->swxa_dma_bufferout = dma_alloc_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,&swxa_dev->pPhysicalOutAddr, GFP_KERNEL);
	if (swxa_dev->swxa_dma_bufferout == NULL)
	{
		printk(KERN_ERR "swxa_dev->swxa_dma_bufferout kmalloc error!\n");
		rc = -ENOMEM;
		goto disable_dev2;
	}
  
	swxa_dev->swxa_dma_bufferKelIn = dma_alloc_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,&swxa_dev->pPhysicalInAddrKel, GFP_KERNEL);
	if (swxa_dev->swxa_dma_bufferKelIn == NULL)
	{
		printk(KERN_ERR "swxa_dev->swxa_dma_bufferKelIn kmalloc error!\n");
		rc = -ENOMEM;
		goto disable_dev3;
	}
	
	swxa_dev->swxa_dma_bufferKelOut = dma_alloc_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,&swxa_dev->pPhysicalOutAddrKel, GFP_KERNEL);
	if (swxa_dev->swxa_dma_bufferKelOut == NULL)
	{
		printk(KERN_ERR "swxa_dev->swxa_dma_bufferKelOut kmalloc error!\n");
		rc = -ENOMEM;
		goto disable_dev4;
	}
	
	swxa_dev->swxa_des_buffer = dma_alloc_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,&swxa_dev->pPhysicalDesInAddr, GFP_KERNEL);
	if (swxa_dev->swxa_des_buffer == NULL)
	{
		printk(KERN_ERR "swxa_dev->swxa_des_buffer kmalloc error!\n");
		rc = -ENOMEM;
		goto disable_dev5;
	}
	
	swxa_dev->swxa_des_bufferout = dma_alloc_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,&swxa_dev->pPhysicalDesOutAddr, GFP_KERNEL);
	if (swxa_dev->swxa_des_bufferout == NULL)
	{
		printk(KERN_ERR "swxa_dev->swxa_des_bufferout kmalloc error!\n");
		rc = -ENOMEM;
		goto disable_dev6;
	}
	
	if (!request_mem_region(swxa_dev->swxa_iobase0, swxa_dev->swxa_io0Len, "swcsm40"))
	{
		rc = -1;
		goto free_all;
	}
	swxa_dev->swxa_membase0 = (unsigned long)ioremap(swxa_dev->swxa_iobase0, swxa_dev->swxa_io0Len);
  //printk(KERN_ERR "3---- swxa_membase0 = 0x%0lx\n", swxa_dev->swxa_membase0);
	
	swxa_dev->swxa_IRQ_NUM = dev->irq;
#ifndef _SWXA_POLLING_
	rc = request_irq(swxa_dev->swxa_IRQ_NUM, swxa_handler, SA_SHIRQ, "swcsm40", swxa_dev);
	if (rc != 0)
	{
		printk(KERN_ERR "Error %d adding swcsm40-%02d", rc, devnum);
		goto unmap_mem;
	}
	
#else
//	disable_irq(swxa_dev->swxa_IRQ_NUM);
#endif

	swxa_dev->swxa_wakeup_flag = 0;
	init_waitqueue_head(&swxa_dev->swxa_wait_on_interrupt);

	pci_set_drvdata(dev, swxa_dev);

	mutex_init(&swxa_dev->swxa_wr_mutex);
	// fSet up the char_dev structure for this device.
  	//	printk(KERN_ERR "5---- pci_set_drvdata OK!\n");
	
	swxa_reset(swxa_dev);
	
	memset(&swxa_dev->cdev, 0, sizeof(swxa_dev->cdev)); //2.6.9内核要求必须清0
	cdev_init(&swxa_dev->cdev, &swxa_fops);
	//printk(KERN_ERR "6---- cdev_init OK!\n");
	swxa_dev->cdev.owner = THIS_MODULE;
	swxa_dev->cdev.ops = &swxa_fops;
	rc = cdev_add(&swxa_dev->cdev, MKDEV(swxa_major, devnum), 1);
	if (rc)
	{
		printk(KERN_ERR "Error %d adding swcsm40-%02d", rc, devnum);
		goto fail;
	}
  	memset(buf, 0, sizeof(buf));
  	if(swxa_dev->type == 40)
		memcpy(buf, "swcsm-pci40-", 12);
	else
		memcpy(buf, "swcsm-pci42-", 12);
	buf[12] = devnum+'0';
	
	if(swxa_dev->type == 40)
		if(t_ECCInit(swxa_dev))
			goto fail;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	class_device_create(swxa_class, NULL, MKDEV(swxa_major, devnum), NULL, buf);
#else
	device_create(swxa_class, NULL, MKDEV(swxa_major, devnum), NULL, buf);
#endif
	
#ifndef _SWXA_POLLING_	
	printk(KERN_ERR "\nSanSec swcsm%02d-%02d Card INTerrupt Driver Installed!\n\n", swxa_dev->type, devnum);
#else
	printk(KERN_ERR "\nSanSec swcsm%02d-%02d Card POLLING Driver Installed!\n\n", swxa_dev->type, devnum);
#endif	
	
	return 0; 
	
fail:
	cdev_del(&swxa_dev->cdev);
	pci_set_drvdata(dev, NULL);
#ifndef _SWXA_POLLING_
	free_irq(swxa_dev->swxa_IRQ_NUM, swxa_dev);
unmap_mem:
#endif	
	iounmap((void*)swxa_dev->swxa_membase0);
	release_mem_region(swxa_dev->swxa_iobase0, swxa_dev->swxa_io0Len);
	
free_all:
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_des_bufferout, swxa_dev->pPhysicalDesOutAddr);
disable_dev6:	
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_des_buffer, swxa_dev->pPhysicalDesInAddr);
disable_dev5:	
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_dma_bufferKelOut, swxa_dev->pPhysicalOutAddrKel);
disable_dev4:
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_dma_bufferKelIn, swxa_dev->pPhysicalInAddrKel);
disable_dev3:
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_dma_bufferout, swxa_dev->pPhysicalOutAddr);
disable_dev2:
	dma_free_coherent(&swxa_dev->pdev->dev, 131072,swxa_dev->swxa_dma_buffer, swxa_dev->pPhysicalInAddr);
disable_dev:
	pci_disable_device(swxa_dev->pdev);
free_part0:	
	kfree(swxa_dev);
	
out:
	Swxa_Dev_Stat[devnum] = 0;
	
	return rc;
}

static void swxa_remove(struct pci_dev *dev)
{
	/* clean up any allocated resources and stuff here.
	 * like call release_region();
	 */
	int minor;
    short type;
    
	struct SWXA_DEV *swxa_dev = pci_get_drvdata(dev);
	
	type = swxa_dev->type;
	minor = MINOR(swxa_dev->cdev.dev);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)	
	class_device_destroy(swxa_class,MKDEV(swxa_major, minor));
#else
	device_destroy(swxa_class,MKDEV(swxa_major, minor));
#endif		
	cdev_del(&swxa_dev->cdev);
	pci_set_drvdata(dev, NULL);
#ifndef _SWXA_POLLING_ 
	free_irq(swxa_dev->swxa_IRQ_NUM, swxa_dev);
#endif
	
	iounmap((void*)swxa_dev->swxa_membase0);
	release_mem_region(swxa_dev->swxa_iobase0, swxa_dev->swxa_io0Len);
	
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_des_bufferout, swxa_dev->pPhysicalDesOutAddr);									
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_des_buffer, swxa_dev->pPhysicalDesInAddr);
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_dma_bufferKelOut, swxa_dev->pPhysicalOutAddrKel);
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_dma_bufferKelIn, swxa_dev->pPhysicalInAddrKel);
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_dma_bufferout, swxa_dev->pPhysicalOutAddr);
	dma_free_coherent(&swxa_dev->pdev->dev, MAX_BUFFER_LEN,swxa_dev->swxa_dma_buffer, swxa_dev->pPhysicalInAddr);
	
	mutex_destroy(&swxa_dev->swxa_wr_mutex);
//	pci_disable_device(swxa_dev->pdev);
#ifndef _SWXA_POLLING_
	#ifdef _SWXA_MSI_	
		pci_disable_msi(swxa_dev->pdev);
	#endif
#endif
	pci_disable_device(swxa_dev->pdev);
	kfree(swxa_dev);
	
	Swxa_Dev_Stat[minor] = 0;
	printk(KERN_ERR "\nSanSec swcsm%02d-%02d Card Driver Uninstalled!\n\n", type, minor);
	
	return;
}

static struct pci_driver pci_driver = {
	.name = "swcsm40",
	.id_table = swxa_ids,
	.probe = swxa_probe,
	.remove = swxa_remove,
};

static int __init swxa_init(void)
{
	int rc=0;
	dev_t dev = 0;
	
	if (swxa_major){
		dev = MKDEV(swxa_major, 0);  //??
		rc = register_chrdev_region(dev, SWXA_MAX_DEV_NUM, "swcsm40");
	}
	if (rc < 0) {
		rc = alloc_chrdev_region(&dev, 0, SWXA_MAX_DEV_NUM, "swcsm40");
		swxa_major = MAJOR(dev);
		if (rc < 0) {
			printk(KERN_ERR "can't get major %d\n", swxa_major);
			return rc;
		}
	}
	
	swxa_class = class_create(THIS_MODULE, "sansec co.ltd 40");
	rc = pci_register_driver(&pci_driver);
	if(rc)
		unregister_chrdev_region(dev, SWXA_MAX_DEV_NUM);
	
	return rc;
}

static void __exit swxa_exit(void)
{
	dev_t dev = MKDEV(swxa_major, 0);
	
	pci_unregister_driver(&pci_driver);
	class_destroy(swxa_class);
	unregister_chrdev_region(dev, SWXA_MAX_DEV_NUM);
}

MODULE_DESCRIPTION("SanSec swxa Card VER:2.0LE");
MODULE_LICENSE("GPL");

module_init(swxa_init);
module_exit(swxa_exit);

