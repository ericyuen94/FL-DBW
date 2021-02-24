/*
 * motrona_IV251_Interface.cpp
 *
 *  Created on: Jun 18, 2012
 *      Author: Loh Yong Hua (KDM/USC)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include "motrona_IV251_Interface.h"
#include "mu_serial.h"


#define	timeout 	0.10
#define buffer_size 	1000

MOTRONA_IV251_INTERFACE::MOTRONA_IV251_INTERFACE()
{

}

MOTRONA_IV251_INTERFACE::~MOTRONA_IV251_INTERFACE()
{

}

int MOTRONA_IV251_INTERFACE::mu_motrona_connect_serial(char *dev)
{
	int i;
	int err;

	/* allocate motrona structure */
	motrona = (mu_motrona_p)calloc(1, sizeof(mu_motrona_t));

	/* open at 9600 baud */
	if(mu_serial_connect(&motrona->fd, dev, 9600) < 0)
	{
		free(motrona);
		return -1;
	}

	err = mu_serial_setallparams(motrona->fd, 9600, 7,'E');
	//printf("[mu_motrona_connect_serial] err:%d\n",err);

	return 0;
}

int MOTRONA_IV251_INTERFACE::mu_motrona_set_enc_pos_readout(void)
{
	unsigned char write_buf[] = {0x04,'1','1',':','8',0x05}; // IV251 serial readout out of actual encoder position
	int write_n = sizeof(write_buf);

	int nwritten = mu_serial_writen(motrona->fd, write_buf, write_n, timeout);

	return nwritten;
}

int MOTRONA_IV251_INTERFACE::mu_motrona_readn(unsigned char *buffer)
{
	int nread = mu_serial_readn(motrona->fd, buffer, 10, timeout);
	//printf("nread:%d\n",nread);

	buffer[nread-1] = 0x00;

	if (nread < 0)
	{
		printf("SERIAL EAGAIN ERROR\n");
		return 0;
	}

	return nread;
}

int MOTRONA_IV251_INTERFACE::mu_motrona_enc_pos_read(void)
{
	unsigned char buffer[100];
	int nread;
	static int ssi_buffer = 0;
	int i, j;
	int s_byte, e_byte;
	unsigned char temp[100];

	// absolute encoder (IV251/ROC413) readout
	mu_motrona_set_enc_pos_readout();
	nread = mu_motrona_readn(buffer);
	//printf("steer_read-> nread = %d\n",nread);
	//for (i=0; i< nread; i++)
	//	printf("%x ",buffer[i]);

	if(nread > 0)
	{
		if ((buffer[0] == 0x2) && (buffer[1] == ':') && (buffer[2] == '8'))
		{
			s_byte = 3;

			for (i=s_byte; i<nread; i++)
			{
				if (buffer[i] == 0x03)
				{
					e_byte = i;
					break;
				}
			}
			j = 0;
			for (i=s_byte; i<e_byte; i++)
			{
				temp[j] = buffer[i+1];
				j++;
			}
			temp[j]='\0';

			ssi_buffer = atoi((char*)temp);
		}
	}

	return ssi_buffer;
}

void MOTRONA_IV251_INTERFACE::mu_motrona_disconnect(void)
{
	if (motrona!=NULL)
	{
		close(motrona->fd);
		free(motrona);
		motrona = NULL;
	}
}

