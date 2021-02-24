/*
 * motrona_IV251_Interface.h
 *
 *  Created on: Jun 18, 2012
 *      Author: Loh Yong Hua (KDM/USC)
 */

#ifndef MU_MOTRONA_H
#define MU_MOTRONA_H

typedef struct {
  unsigned int count;
  double timestamp;
} mu_motrona_data_t, *mu_motrona_data_p;

typedef struct {
  int fd;
  mu_motrona_data_t m_data;
} mu_motrona_t, *mu_motrona_p;

class MOTRONA_IV251_INTERFACE
{
public:
	MOTRONA_IV251_INTERFACE();
	~MOTRONA_IV251_INTERFACE();

	int mu_motrona_connect_serial(char *dev);
	int mu_motrona_set_enc_pos_readout(void);
	int  mu_motrona_readn(unsigned char *buffer);
	int mu_motrona_enc_pos_read(void);
	void mu_motrona_disconnect(void);

private:
	mu_motrona_p motrona;
};

#endif

