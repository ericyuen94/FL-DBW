#ifndef MU_SMARTMOTOR_H
#define MU_SMARTMOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int conn_type;
  int fd;
} mu_smartmotor_t, *mu_smartmotor_p;

mu_smartmotor_p mu_smartmotor_connect_serial(char *dev);

mu_smartmotor_p mu_smartmotor_connect_ethernet(char *host, int port);

void mu_smartmotor_initialize(mu_smartmotor_p sm); //init

void mu_smartmotor_initialize_mt(mu_smartmotor_p sm); //torque mode

void mu_smartmotor_initialize_mv(mu_smartmotor_p sm, int acc, int vel); //velocity mode

void mu_smartmotor_set_abs_pos(mu_smartmotor_p sm, int abs_pos);

void mu_smartmotor_set_relative_pos(mu_smartmotor_p sm, int relative_pos);

void mu_smartmotor_wait(mu_smartmotor_p sm);

void mu_smartmotor_set_torque_val(mu_smartmotor_p sm, int torque_val);

void mu_smartmotor_set_torque_slope_val(mu_smartmotor_p sm, int torque_slope_val);

void mu_smartmotor_set_position_mode(mu_smartmotor_p sm);

void mu_smartmotor_set_torque_mode(mu_smartmotor_p sm);

void mu_smartmotor_set_origin(mu_smartmotor_p sm);

void mu_smartmotor_stop(mu_smartmotor_p sm);

void mu_smartmotor_abruptstop(mu_smartmotor_p sm);

void mu_smartmotor_write(mu_smartmotor_p sm, unsigned char *buffer);

void mu_smartmotor_bytes_write(mu_smartmotor_p sm, unsigned char *buffer, int bytes);

int  mu_smartmotor_read(mu_smartmotor_p sm, unsigned char *buffer);

int  mu_smartmotor_readn(mu_smartmotor_p sm, unsigned char *buffer, int n);

void mu_smartmotor_disconnect(mu_smartmotor_p sm);

void mu_smartmotor_off(mu_smartmotor_p sm);

void mu_smartmotor_wake(mu_smartmotor_p sm);

#ifdef __cplusplus
}
#endif

#endif

