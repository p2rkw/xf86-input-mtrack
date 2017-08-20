/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 **************************************************************************/

#include "trig.h"
#include "common.h"
#include <math.h>

/* Convert a radians value into an mtrack angle.
 */
static double trig_encode_radians(double radians) {
	double angle = (radians / M_PI) * 4.0;
	if (angle < 0.0)
		angle = angle + 8.0;
	return angle;
}

/* Convert an mtrack angle value into radians.
 */
static double trig_decode_radians(double angle) {
	if (angle < 4.0)
		return (angle * M_PI) / 4.0;
	else
		return ((8.0 - angle) * M_PI) / -4.0;
}

double trig_direction(double dx, double dy) {
	if (dx != 0 || dy != 0)
		return trig_encode_radians(atan2(dx, dy*-1));
	return TR_NONE;
}

int trig_generalize(double dir)
{
	if (dir == TR_NONE)
		return TR_NONE;
	else if (dir > 1.0 && dir <= 3.0)
		return TR_DIR_RT;
	else if (dir > 3.0 && dir <= 5.0)
		return TR_DIR_DN;
	else if (dir > 5.0 && dir <= 7.0)
		return TR_DIR_LT;
	else
		return TR_DIR_UP;
}

double trig_angles_add(double a1, double a2)
{
	double a = MODVAL(a1 + a2, 8.0);
	if (a < 0.0)
		a = a + 8.0;
	return a;
}

double trig_angles_sub(double a1, double a2)
{
	return trig_angles_add(a1, -1.0*a2);
}

double trig_angles_acute(double a1, double a2)
{
	double angle;
	if (a1 > a2)
		angle = trig_angles_sub(a1, a2);
	else
		angle = trig_angles_sub(a2, a1);
	if (angle > 4.0)
		angle = 8.0 - angle;
	return angle;
}

double trig_angles_avg(double* angles, int len)
{
	int i;
	double dx, dy, r;
	dx = dy = 0;
	for (i = 0; i < len; i++) {
		r = trig_decode_radians(angles[i]);
		dx += cos(r);
		dy += sin(r);
	}
	return trig_encode_radians(atan2(dy, dx));
}

int trig_angles_cmp(double a1, double a2)
{
	double m1, m2;
	m1 = MODVAL(a1, 8);
	m2 = MODVAL(a2, 8);
	if (m1 == m2)
		return 0;
	else if (m1 > m2)
		return 1;
	else
		return -1;
}
