#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "sockutils.h"
#include "smartmotor.h"
#include "mu_serial.h"
#include "mu_common.h"

#define CONN_SERIAL             0
#define CONN_ETHERNET           1

#define SMARTMOTOR_BUFFER_SIZE  16

mu_smartmotor_p mu_smartmotor_connect_serial(char *dev)
{
	printf("[mu_smartmotor_connect_serial] connecting to %s\n",dev);
	mu_smartmotor_p smartmotor;

	/* allocate smartmotor structure */
	smartmotor = (mu_smartmotor_p)calloc(1, sizeof(mu_smartmotor_t));
	mu_test_alloc(smartmotor);

	/* open at 9600 baud */
	if(mu_serial_connect(&smartmotor->fd, dev, 9600) < 0)
	{
		printf("[mu_smartmotor_connect_serial] FAIL\n");
		free(smartmotor);
		return NULL;
	}
	printf("!!!! [mu_smartmotor_connect_serial] connected\n");
	/* try to switch to 38400 */
	mu_serial_setparams(smartmotor->fd, 9600, 'N');
	//mu_serial_setparams(smartmotor->fd, 38400, 'N');

	smartmotor->conn_type = CONN_SERIAL;

	return smartmotor;
}

mu_smartmotor_p mu_smartmotor_connect_ethernet(char *host, int port)
{
	mu_smartmotor_p smartmotor;

	/* allocate smartmotor structure */
	smartmotor = (mu_smartmotor_p)calloc(1, sizeof(mu_smartmotor_t));
	mu_test_alloc(smartmotor);

	smartmotor->fd = mu_sock_connect(host, port);
	if(smartmotor->fd < 0)
	{
		free(smartmotor);
		return NULL;
	}

	smartmotor->conn_type = CONN_ETHERNET;

	return smartmotor;
}

void fd_writen(mu_smartmotor_p sm, unsigned char *buffer, int n)
{
	int err;

	if(sm->conn_type == CONN_SERIAL)
		err = mu_serial_writen(sm->fd, buffer, n, 0.01);
	else
		err = mu_sock_writen(sm->fd, buffer, n, 0.01);

	if(err < 0)
		mu_warn("Warning: could not send packet to smartmotor\n");
}

int fd_readn(mu_smartmotor_p sm, unsigned char *buffer, int n)
{
	int bytes_read;

	if(sm->conn_type == CONN_SERIAL)
		bytes_read = mu_serial_readn(sm->fd, buffer, n, 0.01);
	else
		bytes_read = mu_sock_readn(sm->fd, buffer, n, 0.01);

	return bytes_read;
}

// Initialise motor in Position Mode
void mu_smartmotor_initialize(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	memset(command,0x00,sizeof(command));
	/*Disable hardware limits*/
	command[0] = 0x45;//E
	command[1] = 0x49;//I
	command[2] = 0x47;//G
	command[3] = 0x4E;//N
	command[4] = 0x28;//(
	command[5] = 0x57;//W
	command[6] = 0x2C;//,
	command[7] = 0x30;//0
	command[8] = 0x29;//)
	command[9] = 0x20;//Space
	fd_writen(sm, (unsigned char *)command, 10);
	memset(command,0x00,sizeof(command));
	usleep(10);
	/* clear errors */
	command[0] = 0x5A;//Z
	command[1] = 0x53;//S
	command[2] = 0x20;//Space
	fd_writen(sm, (unsigned char *)command, 3);
}

// Initialise motor in Torque Mode
void mu_smartmotor_initialize_mt(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/*Disable hardware limits*/
	sprintf(command, "\nEIGN(W,0)\n");
	printf("command = %s", command);
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* clear errors */
	sprintf(command, "\nZS\n");
	printf("command = %s", command);
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* set mode of operation to torque mode */
	sprintf(command, "\nMT\n");
	printf("command = %s", command);
	fd_writen(sm, (unsigned char *)command, strlen(command));
}

// Initialise motor in Velocity Mode
void mu_smartmotor_initialize_mv(mu_smartmotor_p sm, int acc, int vel)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* clear errors */
	sprintf(command, "\nZS\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* set mode of operation to position mode */
	sprintf(command, "\nMV\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* set absolute acceleration */
	sprintf(command, "\nA=%d\n", acc);
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* set maximum permitted velocity */
	sprintf(command, "\nV=%d\n", vel);
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* start motion */
	sprintf(command, "\nG\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
}

// Set absolute position of selected motor
void mu_smartmotor_set_abs_pos(mu_smartmotor_p sm, int abs_pos)
{
	char command[SMARTMOTOR_BUFFER_SIZE];
	int temp=1;
	int count=0;
	int digit[16],digit_num=0, digit_size;

	if(abs_pos>=0){

		while(temp!=0){
			temp = abs_pos/10;
			digit[count] = abs_pos-temp*10;
			abs_pos = temp;
			count++;

		}
		digit_size = count;

		while(count!=0){
			command[digit_num+3] = digit[count-1]+0x30;
			count--;
			digit_num++;

		}

		command[0]=0x50;//P
		command[1]=0x54;//T
		command[2]=0x3D;//=
		command[digit_size+3]=0x20;//SP

		fd_writen(sm, (unsigned char *)command, (digit_size+4));
		//fd_writen(sm, (unsigned char *)command, 10);
		memset(command,0x00,sizeof(command));
		usleep(10);
		command[0]=0x47;//G
		command[1]=0x20;//SP = space
		fd_writen(sm, (unsigned char *)command, 2);

	}
	else{

				abs_pos = abs(abs_pos);

				while(temp!=0){
					temp = abs_pos/10;
					digit[count] = abs_pos-temp*10;
					abs_pos = temp;
					count++;

				}
				digit_size = count;

				while(count!=0){
					command[digit_num+4] = digit[count-1]+0x30;
					count--;
					digit_num++;

				}

				command[0]=0x50;//P
				command[1]=0x54;//T
				command[2]=0x3D;//=
				command[3]=0x2D;//-
				command[digit_size+4]=0x20;

				fd_writen(sm, (unsigned char *)command, (digit_size+5));
				//fd_writen(sm, (unsigned char *)command, 10);
				memset(command,0x00,sizeof(command));
				usleep(10);
				command[0]=0x47;//G
				command[1]=0x20;//SP
				fd_writen(sm, (unsigned char *)command, 2);
	}

	//send cmd to ensure steer motor hits the desired steer angle.
	memset(command,0x00,sizeof(command));
	usleep(10);
	command[0]=0x54;//T
	command[1]=0x57;//W
	command[2]=0x41;//A
	command[3]=0x49;//I
	command[4]=0x54;//T
	command[5]=0x20;//SP
	fd_writen(sm, (unsigned char *)command, 6);

//	command[0]=0x50;//P
//	command[1]=0x54;//T
//	command[2]=0x3D;//=
//	command[3]=0x31;//
//	command[4]=0x30;
//	command[5]=0x30;
//	command[6]=0x30;
//	command[7]=0x30;
//	command[8]=0x30;
//	command[9]=0x20;
//	fd_writen(sm, (unsigned char *)command, 10);
//	memset(command,0x00,sizeof(command));
//	usleep(10);
//	command[0]=0x47;
//	command[1]=0x20;
//	fd_writen(sm, (unsigned char *)command, 2);

	/* clear errors */
//	sprintf(command, "\nZS\n");
//	fd_writen(sm, (unsigned char *)command, strlen(command));
//	//
//	/* set absolute position for move */
	//abs_pos = 100000;
//	char int_ascii_buffer[20];
//	sprintf(command, "\nPT=%c\n", itoa(abs_pos,int_ascii_buffer));
//	printf("%s ", int_ascii_buffer);


	/* clear errors */
//	sprintf(command, "\nZS\n");
//	fd_writen(sm, (unsigned char *)command, strlen(command));
//	/* set absolute position for move */
//	sprintf(command, "\nP=%d\n", abs_pos);
//	//sprintf(command, "\nPT=%d\n", abs_pos);
//	fd_writen(sm, (unsigned char *)command, strlen(command));
//	/* start motion */
//	sprintf(command, "\nG\n");
//	fd_writen(sm, (unsigned char *)command, strlen(command));
}

// Set relative position of selected motor
void mu_smartmotor_set_relative_pos(mu_smartmotor_p sm, int relative_pos)
{
	char command[SMARTMOTOR_BUFFER_SIZE];
	/* clear errors */
	sprintf(command, "\nZS\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* set relative distance for position move */
	sprintf(command, "\nD=%d\n", relative_pos);
	fd_writen(sm, (unsigned char *)command, strlen(command));
	/* start motion */
	sprintf(command, "\nG\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
}

void mu_smartmotor_wait(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];
	/* clear errors */
	sprintf(command, "\nTWAIT\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
}


void mu_smartmotor_set_torque_val(mu_smartmotor_p sm, int torque_val)
{
	char command[SMARTMOTOR_BUFFER_SIZE];
	int digit[16],digit_num=0, digit_size;
	int temp=1;
	int count=0;

	/* set torque value for torque mode */
	memset(command,0x00,sizeof(command));
	if(torque_val>=0){

		while(temp!=0){
			temp = torque_val/10;
			digit[count] = torque_val-temp*10;
			torque_val = temp;
			count++;

		}
		digit_size = count;

		while(count!=0){
			command[digit_num+2] = digit[count-1]+0x30;
			count--;
			digit_num++;

		}

		command[0]=0x54;//T
		command[1]=0x3D;//=
		command[digit_size+3]=0x20;//SP

		printf("command = %s\n", command);

		fd_writen(sm, (unsigned char *)command, (digit_size+4));
		//fd_writen(sm, (unsigned char *)command, 10);
		memset(command,0x00,sizeof(command));
		usleep(10);
		command[0]=0x47;//G
		command[1]=0x20;//SP = space
		fd_writen(sm, (unsigned char *)command, 2);

	}
	else{

		torque_val = abs(torque_val);

		while(temp!=0){
			temp = torque_val/10;
			digit[count] = torque_val-temp*10;
			torque_val = temp;
			count++;

		}
		digit_size = count;

		while(count!=0){
			command[digit_num+3] = digit[count-1]+0x30;
			count--;
			digit_num++;

		}

		command[0]=0x54;//T
		command[1]=0x3D;//=
		command[2]=0x2D;//-
		command[digit_size+4]=0x20;

		fd_writen(sm, (unsigned char *)command, (digit_size+5));
		memset(command,0x00,sizeof(command));
		usleep(10);
		command[0]=0x47;//G
		command[1]=0x20;//SP
		fd_writen(sm, (unsigned char *)command, 2);
	}
}

void mu_smartmotor_set_torque_slope_val(mu_smartmotor_p sm, int torque_slope_val)
{
	char command[SMARTMOTOR_BUFFER_SIZE];
	int digit[16],digit_num=0, digit_size;
	int temp=1;
	int count=0;

	/* set torque value for torque mode */
	memset(command,0x00,sizeof(command));
	if(torque_slope_val>=0){

		while(temp!=0){
			temp = torque_slope_val/10;
			digit[count] = torque_slope_val-temp*10;
			torque_slope_val = temp;
			count++;

		}
		digit_size = count;

		while(count!=0){
			command[digit_num+3] = digit[count-1]+0x30;
			count--;
			digit_num++;

		}

		command[0]=0x54;//T
		command[1]=0x53;//S
		command[2]=0x3D;//=
		command[digit_size+3]=0x20;//SP

		printf("command = %s\n", command);

		fd_writen(sm, (unsigned char *)command, (digit_size+4));
		//fd_writen(sm, (unsigned char *)command, 10);
		memset(command,0x00,sizeof(command));
		usleep(10);
		command[0]=0x47;//G
		command[1]=0x20;//SP = space
		fd_writen(sm, (unsigned char *)command, 2);

	}
	else if(torque_slope_val<0){

		torque_slope_val = abs(torque_slope_val);

		while(temp!=0){
			temp = torque_slope_val/10;
			digit[count] = torque_slope_val-temp*10;
			torque_slope_val = temp;
			count++;

		}
		digit_size = count;

		while(count!=0){
			command[digit_num+4] = digit[count-1]+0x30;
			count--;
			digit_num++;

		}

		command[0]=0x54;//T
		command[1]=0x53;//S
		command[2]=0x3D;//=
		command[3]=0x2D;//-
		command[digit_size+4]=0x20;

		fd_writen(sm, (unsigned char *)command, (digit_size+5));
		memset(command,0x00,sizeof(command));
		usleep(10);
		command[0]=0x47;//G
		command[1]=0x20;//SP
		fd_writen(sm, (unsigned char *)command, 2);
	}
}

void mu_smartmotor_set_position_mode(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* set position mode */
	memset(command,0x00,sizeof(command));
	command[0] = 0x4D;//M
	command[1] = 0x50;//P
	command[2] = 0x20;//Space
	fd_writen(sm, (unsigned char *)command, strlen(command));
	memset(command,0x00,sizeof(command));
	usleep(10);
	command[0] = 0x41;//A
	command[1] = 0x44;//D
	command[2] = 0x54;//T
	command[3] = 0x3D;//=
	command[4] = 0x31;//1
	command[5] = 0x30;//0
	command[6] = 0x30;//0
	command[7] = 0x30;//0
	command[8] = 0x20;//Space
	fd_writen(sm, (unsigned char *)command, strlen(command));
	memset(command,0x00,sizeof(command));
	usleep(10);
	command[0] = 0x56;//V
	command[1] = 0x54;//T
	command[2] = 0x3D;//=
	command[3] = 0x38;//5
	command[4] = 0x30;//0
	command[5] = 0x30;//0
	command[6] = 0x30;//0
	command[7] = 0x30;//0
	command[8] = 0x30;//0
	command[9] = 0x20;//Space
	fd_writen(sm, (unsigned char *)command, strlen(command));

}

void mu_smartmotor_set_torque_mode(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* set torque mode */
	memset(command,0x00,sizeof(command));
	command[0] = 0x4D;//M
	command[1] = 0x54;//T
	command[2] = 0x20;//Space
	fd_writen(sm, (unsigned char *)command, strlen(command));
}

void mu_smartmotor_set_origin(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* reset internal encoder position */
	sprintf(command, "\nO=0\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
}

void mu_smartmotor_stop(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* decelerate to stop */
	memset(command,0x00,sizeof(command));
	command[0] = 0x53;//X
	command[1] = 0x20;//Space
	fd_writen(sm, (unsigned char *)command, strlen(command));
}

void mu_smartmotor_abruptstop(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* abruptly stop */
	sprintf(command, "\nS\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
}

void mu_smartmotor_off(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* release motor brake */
	sprintf(command, "\nBRKRLS\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
	mu_sleep(1);
	/* turn off servo */
	sprintf(command, "\nOFF\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
	mu_sleep(1);
}

void mu_smartmotor_wake(mu_smartmotor_p sm)
{
	char command[SMARTMOTOR_BUFFER_SIZE];

	/* release motor brake */
	sprintf(command, "\nWAKE\n");
	fd_writen(sm, (unsigned char *)command, strlen(command));
	mu_sleep(1);
}

void mu_smartmotor_bytes_write(mu_smartmotor_p sm, unsigned char *buffer, int bytes)
{
	fd_writen(sm, (unsigned char *)buffer, bytes);
}

void mu_smartmotor_write(mu_smartmotor_p sm, unsigned char *buffer)
{
	fd_writen(sm, (unsigned char *)buffer, strlen(buffer));
}

int mu_smartmotor_read(mu_smartmotor_p sm, unsigned char *buffer)
{
	return fd_readn(sm, (unsigned char *)buffer, SMARTMOTOR_BUFFER_SIZE);
}

int mu_smartmotor_readn(mu_smartmotor_p sm, unsigned char *buffer, int n)
{
	return fd_readn(sm, (unsigned char *)buffer, n);
}

void mu_smartmotor_disconnect(mu_smartmotor_p sm)
{
	close(sm->fd);
	free(sm);
}

