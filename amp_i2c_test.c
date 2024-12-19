#include <stdio.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "i2c_driver.h"

#define DEBUG_MODE 1
#define BUS_NUM_PARAM 1
#define READ_WRITE_PARAM 2
#define DEVICE_ID 3
#define REGADDR 4
#define READ_LEN 5
#define VALUE_START 5
#define BATCH_FILE_PARAM 3

#define MAX_CMD_DATA_LENTH 30
#define MAX_LINE_LENTH 50

static int amp_fd;

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
unsigned char cmd_file_parse(unsigned char *cmd_buf, unsigned char *data, unsigned char cmd_buf_len)
{
	unsigned char lenth = 0;
	for (int i = 0; i < cmd_buf_len; i++)
	{
		if (cmd_buf[i] == ' ')
		{
			data[lenth] = strtoul(&cmd_buf[i + 1], NULL, 16);
			// printf("data[%d] = 0x%02x\n", lenth, data[lenth]);
			lenth++;
		}
		else
		{
			continue;
		}
	}
	return lenth;
}
int batch_file_handle(char *batch_file_name)
{

	printf("%s\n", __func__);
	FILE *batch_file = fopen(batch_file_name, "r");
	if (batch_file == NULL)
	{
		printf("read batch cmd file fail\n");
		return -1;
	}
	unsigned char buf[MAX_LINE_LENTH] = {0};
	while (fgets(buf, MAX_LINE_LENTH, batch_file))
	{
		if ((buf[0] != 'w') && (buf[0] != 'r') && (buf[0] != 'd'))
		{
			// printf("is not a correct cmd\n");
			continue;
		}
		else
		{
			switch (buf[0])
			{
			case 'w':
			{
				printf("write cmd\n");
				unsigned char data[MAX_CMD_DATA_LENTH] = {0};
				unsigned lenth = cmd_file_parse(&buf[1], data, strlen(buf));
#if !DEBUG_MODE
				i2c_write_buf(amp_fd, data[0] / 2, data[1], &data[2], lenth - 2);
#endif
				break;
			}
			case 'r':
			{
				printf("read cmd\n");
				unsigned char data[MAX_CMD_DATA_LENTH] = {0};
				unsigned lenth = cmd_file_parse(&buf[1], data, strlen(buf));
				unsigned char *read_buf = malloc(data[lenth - 1]);
				if (read_buf == NULL)
				{
					printf("malloc fail\n");
					return -1;
				}
#if !DEBUG_MODE
				i2c_read_buf(amp_fd, data[0] / 2, data[1], read_buf, data[lenth - 1]);
#endif
				free(read_buf);
				break;
			}
			case 'd':
			{
				unsigned long int delay = strtoul(&buf[2], NULL, 10);
				printf("delay %ldms\n", delay);
				usleep(delay * 1000);
				break;
			}
			}
		}
	}
}
int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		help_info();
		return 0;
	}
#if !DEBUG_MODE
	amp_fd = i2c_init(strtoul(argv[BUS_NUM_PARAM], NULL, 10));
	if (amp_fd < 0)
	{
		return -1;
	}
#endif

	// printf("deviceid = 0x%02x regaddr = 0x%02x\n",device_id,regaddr);
	if (!strcmp(argv[READ_WRITE_PARAM], "w"))
	{
		// printf("write cmd\n");
		unsigned char device_id = strtoul(argv[DEVICE_ID], NULL, 16) / 2;
		unsigned char regaddr = strtoul(argv[REGADDR], NULL, 16);
		unsigned value_len = argc - VALUE_START;
		unsigned char *buf = malloc(value_len);
		if (buf == NULL)
		{
			printf("malloc fail\n");
			return -1;
		}
		printf("write regaddr 0x%02x value = ", regaddr);
		for (int i = 0; i < value_len; i++)
		{
			buf[i] = strtoul(argv[i + VALUE_START], NULL, 16);
			printf("0x%02x ", buf[i]);
		}
		printf("\n");
		i2c_write_buf(amp_fd, device_id, regaddr, buf, value_len);
		free(buf);
	}
	else if (!strcmp(argv[READ_WRITE_PARAM], "r"))
	{
		// printf("read cmd\n");
		unsigned char device_id = strtoul(argv[DEVICE_ID], NULL, 16) / 2;
		unsigned char regaddr = strtoul(argv[REGADDR], NULL, 16);
		unsigned value_len = strtoul(argv[READ_LEN], NULL, 16);
		unsigned char *buf = malloc(value_len);
		if (buf == NULL)
		{
			printf("malloc fail\n");
			return -1;
		}
		printf("read regaddr 0x%02x value = ", regaddr);
		i2c_read_buf(amp_fd, device_id, regaddr, buf, value_len);
		for (int i = 0; i < value_len; i++)
		{
			printf("0x%02x ", buf[i]);
		}
		printf("\n");
		free(buf);
	}
	else if (!strcmp(argv[READ_WRITE_PARAM], "-f"))
	{
		batch_file_handle(argv[BATCH_FILE_PARAM]);
	}
	else
	{
		printf("unknown cmd\n");
	}
	return 0;
}