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

/* Some bastardized trig functions. These calculations flip the
 * Y axis since that axis on touchpads is opposite that of the
 * Cartesian system.
 */

#ifndef MT_TRIG_H
#define MT_TRIG_H

#define TR_NONE -1
#define TR_DIR_UP 0
#define TR_DIR_RT 2
#define TR_DIR_DN 4
#define TR_DIR_LT 6

/* Determine the direction of a vector. This uses the slope of the
 * vector to approximate the angle, as such it is only accurate at
 * increments of 45 degrees. This is sufficient for our uses.
 *
 * The returned value is 0 <= a < 8 such that the circle is split
 * into 45 degree sections. Each whole number lies 45 degrees apart
 * and so whole numbers are exact. All fractional parts are
 * aproximations.
 *
 * TR_NONE will be returned if the magnitude of the vector is zero.
 */
double trig_direction(double dx, double dy);

/* Generalize a direction.  Returns TR_NONE, TR_DIR_UP, TR_DIR_RT,
 * TR_DIR_DN, or TR_DIR_LT.
 */
int trig_generalize(double dir);

/* Add two angles.
 */
double trig_angles_add(double a1, double a2);

/* Subtract two angles.
 */
double trig_angles_sub(double a1, double a2);

/* Calculate the acute angle between two angles.
 */
double trig_angles_acute(double a1, double a2);

/* Average a collection of angles.
 */
double trig_angles_avg(double* angles, int len);

/* Compare two angles. Returns 0 if a1 == a2. Returns < 0 if a1 < a2.
 * Returns > 0 if a1 > a2.
 */
int trig_angles_cmp(double a1, double a2);

#endif

