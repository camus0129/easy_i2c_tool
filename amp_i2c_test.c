#include<stdio.h>
#include<linux/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<errno.h>
#include<assert.h>
#include<string.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>

#define BUS_NUM_PARAM 1
#define READ_WRITE_PARAM 2
#define DEVICE_ID  3
#define REGADDR 4
#define READ_LEN 5
#define VALUE_START 5

static int amp_fd;
int i2c_init(int bus_num)
{
	char buf[20]={0};
	sprintf(buf,"/dev/i2c-%d",bus_num);
	int i2c_fd = open(buf, O_RDWR);
	if (i2c_fd < 0)
	{
		printf("open %s fail\n",buf);
		return -1;
	}
	return i2c_fd;
}

int i2c_write_buf(unsigned char chip_addr, unsigned char reg_addr, unsigned char *buf, unsigned char len)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msg;
	int ret;
	work_queue.msgs = &msg;
	work_queue.nmsgs = 1;
	msg.len = len + 1;
	msg.flags = 0;	//write
	msg.addr = chip_addr;
	msg.buf = (unsigned char *)malloc(len + 1);
	msg.buf[0] = reg_addr;
	memcpy(&msg.buf[1], buf, len);

	ret = ioctl(amp_fd, I2C_RDWR, (unsigned long)&work_queue);
	if (ret < 0)
		printf("error during I2C_RDWR ioctl with error code %d\n", ret);
	return ret;
}

int i2c_read_buf(unsigned char chip_addr, unsigned char reg_addr, unsigned char *buf, unsigned char len)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msg[2];
	int ret;
	work_queue.nmsgs  = 2;
	work_queue.msgs = msg;
	msg[0].len = 1;
	msg[0].flags = 0;
	msg[0].addr = chip_addr;
	msg[0].buf = &reg_addr;

	msg[1].len = len;
	msg[1].flags = I2C_M_RD;
	msg[1].addr = chip_addr;
	msg[1].buf = buf;
	ret = ioctl(amp_fd, I2C_RDWR, (unsigned long)&work_queue);
	if (ret < 0)
		printf("error during I2C_RDWR ioctl with error code %d\n", ret);
	return ret;
}
int i2c_write_byte(unsigned char chip_addr,unsigned char reg_addr,unsigned char value)
{
	return i2c_write_buf(chip_addr,reg_addr,&value,1);
}
void help_info(void)
{
	printf("Useage:\n");
	printf("\n1. read write directly:\n");
	printf("amp_i2c_test bus-num w/r device regaddr value/length\n");
	printf("example:\n");
	printf("amp_i2c_test 2 w 0x58 0x00 0x01 //write dev=0x58 reg=0x00,value=0x01 in  /dev/i2c-2 \n");
	printf("amp_i2c_test 2 r 0x58 0x00 0x01 //read dev=0x58 reg=0x00,lenth=0x01 in /dev/i2c-2\n");
	printf("\n2. load read/write cmd from batch file:\n");
	printf("amp_i2c_test bus-num -f cmd batch file\n");
}
int main(int argc,char *argv[])
{
	if(argc < 4)
	{
		help_info();
		return 0;
	}
	amp_fd = i2c_init(strtoul(argv[BUS_NUM_PARAM],NULL,10)); 
	if(amp_fd < 0)
	{
		return -1;
	}
	unsigned char device_id = strtoul(argv[DEVICE_ID],NULL,16)/2;
	unsigned char regaddr = strtoul(argv[REGADDR],NULL,16);
	//printf("deviceid = 0x%02x regaddr = 0x%02x\n",device_id,regaddr);
	if(!strcmp(argv[READ_WRITE_PARAM],"w"))
		{
			//printf("write cmd\n");
			unsigned value_len = argc - VALUE_START;
			unsigned char *buf=malloc(value_len);
			printf("write regaddr 0x%02x value = ",regaddr);
			for(int i=0;i<value_len;i++)
			{
				buf[i] = strtoul(argv[i+VALUE_START],NULL,16);
				printf("0x%02x ",buf[i]);
			}
			printf("\n");
			i2c_write_buf(device_id,regaddr,buf,value_len);
		}
	else if(!strcmp(argv[READ_WRITE_PARAM],"r"))
	{
		//printf("read cmd\n");
		unsigned value_len = strtoul(argv[READ_LEN],NULL,16);
		unsigned char *buf = malloc(value_len);
		printf("read regaddr 0x%02x value = ",regaddr);
		i2c_read_buf(device_id,regaddr,buf,value_len);
		for(int i=0;i<value_len;i++)
		{
			printf("0x%02x ",buf[i]);
		}
		printf("\n");
	}
	else
	{
		printf("unknown cmd\n");
	}
	return 0;
}