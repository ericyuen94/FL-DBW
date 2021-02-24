/*
 * mu_common.h
 *
 *  Created on: Mar 23, 2012
 *      Author: Loh Yong Hua (USC,KDM)
 */

#ifndef MU_COMMON_H_
#define MU_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define      M_PI           3.14159265358979323846
#endif

static FILE *mu_carp_output = NULL;

double mu_square(double x);

double mu_fmin(double val1, double val2);

double mu_fmax(double val1, double val2);

double mu_d2r(double theta);

double mu_ms2kph(double ms);

void mu_sleep(double sleep_time);

void mu_kill(const char* fmt, ...);

void mu_warn(const char* fmt, ...);

#define mu_test_alloc(X) do {if ((void *)(X) == NULL) mu_kill("Out of memory in %s, (%s, line %d).\n", __FUNCTION__, __FILE__, __LINE__); } while (0)

#ifdef __cplusplus
}
#endif

#endif /* MU_COMMON_H_ */
