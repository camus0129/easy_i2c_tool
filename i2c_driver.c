/**********************************************************
File  Name: 		i2c_driver.c

Description:        I2C diver implement

Auther: 			Sven

Create Date: 		2024-12-19 14:43:36
***********************************************************/

/**********************************************************
Header Files Include
***********************************************************/
#include "i2c_driver.h"
/**********************************************************
MACRO Defines
***********************************************************/

/**********************************************************
Data Type defines
***********************************************************/

/**********************************************************
Global Variabl Defines
***********************************************************/

/**********************************************************
Function Defines
***********************************************************/
int i2c_init(int bus_num)
{
    char buf[20] = {0};
    sprintf(buf, "/dev/i2c-%d", bus_num);
    int i2c_fd = open(buf, O_RDWR);
    if (i2c_fd < 0)
    {
        printf("open %s fail\n", buf);
        return -1;
    }
    return i2c_fd;
}

int i2c_write_buf(int device_fd, unsigned char chip_addr, unsigned char reg_addr, unsigned char *buf, unsigned char len)
{
    struct i2c_rdwr_ioctl_data work_queue;
    struct i2c_msg msg;
    int ret;
    work_queue.msgs = &msg;
    work_queue.nmsgs = 1;
    msg.len = len + 1;
    msg.flags = 0; // write
    msg.addr = chip_addr;
    msg.buf = (unsigned char *)malloc(len + 1);
    if (msg.buf == NULL)
    {
        printf("malloc fail\n");
        return -1;
    }
    msg.buf[0] = reg_addr;
    memcpy(&msg.buf[1], buf, len);

    ret = ioctl(device_fd, I2C_RDWR, (unsigned long)&work_queue);
    if (ret < 0)
        printf("error during I2C_RDWR ioctl with error code %d\n", ret);
    free(msg.buf);
    return ret;
}

int i2c_read_buf(int device_fd, unsigned char chip_addr, unsigned char reg_addr, unsigned char *buf, unsigned char len)
{
    struct i2c_rdwr_ioctl_data work_queue;
    struct i2c_msg msg[2];
    int ret;
    work_queue.nmsgs = 2;
    work_queue.msgs = msg;
    msg[0].len = 1;
    msg[0].flags = 0;
    msg[0].addr = chip_addr;
    msg[0].buf = &reg_addr;

    msg[1].len = len;
    msg[1].flags = I2C_M_RD;
    msg[1].addr = chip_addr;
    msg[1].buf = buf;
    ret = ioctl(device_fd, I2C_RDWR, (unsigned long)&work_queue);
    if (ret < 0)
        printf("error during I2C_RDWR ioctl with error code %d\n", ret);
    return ret;
}
int i2c_write_byte(int ddevice_fd, unsigned char chip_addr, unsigned char reg_addr, unsigned char value)
{
    return i2c_write_buf(ddevice_fd, chip_addr, reg_addr, &value, 1);
}