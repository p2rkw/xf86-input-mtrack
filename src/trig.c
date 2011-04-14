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

/* Determine the angle of a vector within the given quadrant.
 * All this really does is calculate the slope in relation to
 * which quadrant it is in.
 */
static double trig_quadrant_angle(int quadrant, double dx, double dy) {
	dx = ABSVAL(dx);
	dy = ABSVAL(dy);
	if (quadrant == 1 || quadrant == 3)
		return dx < dy ? 2-dx/dy : dy/dx;
	else
		return dy < dx ? 2-dy/dx : dx/dy;
}

int trig_quadrant(double dx, double dy)
{
	if (dx > 0 && dy < 0)
		return 0;
	else if (dx > 0 && dy > 0)
		return 1;
	else if (dx < 0 && dy > 0)
		return 2;
	else if (dx < 0 && dy < 0)
		return 3;
	else
		return -1;
}

double trig_direction(double dx, double dy)
{
	int qn;
	if (dx == 0 && dy == 0)
		return TR_NONE;
	else if (dx == 0)
		return (dy < 0) ? 0 : 4;
	else if (dy == 0)
		return (dx > 0) ? 2 : 6;
	else {
		qn = trig_quadrant(dx, dy);
		return qn*2 + trig_quadrant_angle(qn, dx, dy);
	}
}

int trig_generalize(double dir)
{
	if (dir == -1)
		return TR_NONE;
	else if (dir > 1 && dir <= 3)
		return TR_DIR_RT;
	else if (dir > 3 && dir <= 5)
		return TR_DIR_DN;
	else if (dir > 5 && dir <= 7)
		return TR_DIR_LT;
	else
		return TR_DIR_UP;
}

double trig_angles_add(double a1, double a2)
{
	double a = MODVAL(a1 + a2, 8.0);
	if (a < 0)
		a = a + 8.0;
	return a;
}

double trig_angles_sub(double a1, double a2)
{
	return trig_angles_add(a1, -1.0*a2);
}

double trig_angles_acute(double a1, double a2)
{
	if (a1 > a2)
		return trig_angles_sub(a1, a2);
	else
		return trig_angles_sub(a2, a1);
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

