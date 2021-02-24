/*
 * mu_common.c
 *
 *  Created on: Mar 23, 2012
 *      Author: Loh Yong Hua (USC,KDM)
 */
#include "mu_common.h"

double mu_square(double x)
{
	return (x * x);
}

double mu_fmin(double val1, double val2)
{
	if (val2 < val1)
		return val2;
	return val1;
}

double mu_fmax(double val1, double val2)
{
	if (val2 > val1)
		return val2;
	return val1;
}

double mu_d2r(double theta)
{
	return (theta * M_PI / 180.0);
}

double mu_ms2kph(double ms)
{
	return (ms * 3.6);
}

void mu_sleep(double sleep_time)
{
	usleep(sleep_time*1000000);
}

void mu_kill(const char* fmt, ...)
{
	va_list args;

	if (mu_carp_output == NULL)
		mu_carp_output = stderr;

	va_start(args, fmt);
	vfprintf(mu_carp_output, fmt, args);
	va_end(args);
	fflush(mu_carp_output);
	exit(0);
}

void mu_warn(const char* fmt, ...)
{
	va_list args;

	if (mu_carp_output == NULL)
		mu_carp_output = stderr;

	va_start(args, fmt);
	vfprintf(mu_carp_output, fmt, args);
	va_end(args);
	fflush(mu_carp_output);
}
