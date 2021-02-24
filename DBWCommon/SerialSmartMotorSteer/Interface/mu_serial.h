#ifndef MU_SERIAL_H
#define MU_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* connect to a serial port at a given speed */

int mu_serial_connect(int *fd, char *dev_name, int baudrate);

/* set the buadrate and the parity of the serial port - doesn't currently
   support flow control.  Uses 8 data bits and 1 stop bit */

int mu_serial_setparams(int fd, int baudrate, char parity);

/* set the buadrate, databits and the parity of the serial port - doesn't 
   currently support flow control.  Uses 1 stop bit */

int mu_serial_setallparams(int fd, int baudrate, int databits, char parity);

/* returns the number of bytes available to be read on a serial channel */    
    
long int mu_serial_bytes_available(int fd);

/* clear the input buffer of a serial channel */

int mu_serial_clear_input_buffer(int fd);

/* writes n characters to a serial channel. Timeout applies to each
   individual write, not the total write time */

int mu_serial_writen(int fd, unsigned char *buffer, int n, double timeout);

/* reads n characters from a serial channel unless a timeout or 
   error occurs */

int mu_serial_readn(int fd, unsigned char *buffer, int n, double timeout);

/* closes the serial port */

void mu_serial_close(int fd);

#ifdef __cplusplus
}
#endif

#endif
