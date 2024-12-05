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

#define DEV_BUS "/dev/i2c-2"
static int amp_fd;
int i2c_init(char *dev_bus)
{
	int i2c_fd = open(dev_bus, O_RDWR);
	if (i2c_fd < 0)
	{
		printf("open %s fail\n",dev_bus);
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
int main(int argc,char *argv[])
{
	if(argc < 4)
	{
		printf("Useage:amp_i2c_test w/r device regaddr value\n");
		return 0;
	}
	amp_fd = i2c_init(DEV_BUS); 
	if(amp_fd < 0)
	{
		return -1;
	}
	unsigned char device_id = strtoul(argv[2],NULL,16)/2;
	unsigned char regaddr = strtoul(argv[3],NULL,16);
	printf("deviceid = 0x%02x regaddr = 0x%02x\n",device_id,regaddr);
	if(!strcmp(argv[1],"w"))
		{
			printf("write cmd\n");
			unsigned value_len = argc - 4;
			unsigned char *buf=malloc(value_len);
			for(int i=0;i<value_len;i++)
			{
				buf[i] = strtoul(argv[i+4],NULL,16);
				printf(" write value[%d]=0x%02x\n",i,buf[i]);
			}
			i2c_write_buf(device_id,regaddr,buf,value_len);
		}
	else if(!strcmp(argv[1],"r"))
	{
		printf("read cmd\n");
		unsigned value_len = strtoul(argv[4],NULL,16);
		unsigned char *buf = malloc(value_len);
		printf("read len = %d\n",value_len);
		i2c_read_buf(device_id,regaddr,buf,value_len);
		for(int i=0;i<value_len;i++)
		{
			printf("read value[%d]=0x%02x\n",i,buf[i]);
		}
	}
	else
	{
		printf("unknown cmd\n");
	}
	return 0;
}