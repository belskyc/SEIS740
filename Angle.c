/*
 *  Angle.c
 *
 *  Created on: Apr 21, 2013
 *      Author: jhatch
 */

#include <stdio.h>

#define UNIT_TEST 0

typedef struct AngleVal
{
    int		deg;
	double	tan;
} AngleVal_t;

AngleVal_t AngleTable[] = { { 0, 0.0000 },
							{ 1, 0.0175 },
							{ 2, 0.0349 },
							{ 3, 0.0524 },
							{ 4, 0.0699 },
							{ 5, 0.0875 },
							{ 6, 0.1051 },
							{ 7, 0.1228 },
							{ 8, 0.1405 },
							{ 9, 0.1584 },
							{10, 0.1763 },
							{11, 0.1944 },
							{12, 0.2126 },
							{13, 0.2309 },
							{14, 0.2493 },
							{15, 0.2679 },
							{16, 0.2867 },
							{17, 0.3057 },
							{18, 0.3249 },
							{19, 0.3443 },
							{20, 0.3640 },
							{21, 0.3839 },
							{22, 0.4040 },
							{23, 0.4245 },
							{24, 0.4452 },
							{25, 0.4663 },
							{26, 0.4877 },
							{27, 0.5095 },
							{28, 0.5317 },
							{29, 0.5543 },
							{30, 0.5774 },
							{31, 0.6009 },
							{32, 0.6249 },
							{33, 0.6494 },
							{34, 0.6745 },
							{35, 0.7002 },
							{36, 0.7265 },
							{37, 0.7536 },
							{38, 0.7813 },
							{39, 0.8098 },
							{40, 0.8391 },
							{41, 0.8693 },
							{42, 0.9004 },
							{43, 0.9325 },
							{44, 0.9657 },
							{45, 1.0000 },
							{46, 1.0355 },
							{47, 1.0724 },
							{48, 1.1106 },
							{49, 1.1504 },
							{50, 1.1918 },
							{51, 1.2349 },
							{52, 1.2799 },
							{53, 1.3270 },
							{54, 1.3764 },
							{55, 1.4281 },
							{56, 1.4826 },
							{57, 1.5399 },
							{58, 1.6003 },
							{59, 1.6643 },
							{60, 1.7321 },
							{61, 1.8040 },
							{62, 1.8807 },
							{63, 1.9626 },
							{64, 2.0503 },
							{65, 2.1445 },
							{66, 2.2460 },
							{67, 2.3559 },
							{68, 2.4751 },
							{69, 2.6051 },
							{70, 2.7475 },
							{71, 2.9042 },
							{72, 3.0777 },
							{73, 3.2709 },
							{74, 3.4874 },
							{75, 3.7321 },
							{76, 3.0108 },
							{77, 4.3315 },
							{78, 4.7046 },
							{79, 5.1446 },
							{80, 5.6713 },
							{81, 6.3138 },
							{82, 7.1154 },
							{83, 8.1443 },
							{84, 9.5144 },
							{85, 11.4301},
							{86, 14.3007},
							{87, 19.0811},
							{88, 28.6363},
							{89, 57.2900},
							{90, 1000000}
						  };

int binarySearch(double val)
{
    int x, L, R;
	L = 0; R = 90;

	do
	{
		x = (L + R) / 2;
		if (val < AngleTable[x].tan)
			//R = x - 1; // original
			R = x;
		else
			L = x;
			//L = x + 1; // original
	} while ((AngleTable[x].tan != val) && (L < R) && ((R-L) > 1));

	x = (L + R) / 2;
	return AngleTable[x].deg;
}

int GetAngle(int x, int y)
{
    int xVal = 0;
	// int yVal = 0;
	int neg = 0; // negative flag
	int deg = 0; // ret val in degrees
	double ratio;

	if (x < 0)
	{
		neg = 1;
		xVal = -1 * x;
	}

	ratio = (double)x / (double)y;

	deg = binarySearch(ratio);

	if (neg)
		deg = -1 * deg;

	return deg;
}

#if UNIT_TEST
void main( int argc, char *argv[] )
{
    int x, y, deg;

	deg = 0;
	x = 5;
	y = 7;
	deg = GetAngle(x, y);
	printf ("x = %d, y = %d, Angle = %d\n", x, y, deg);

	x = 6; y = 3; deg = 0;
	deg = GetAngle(x, y);
	printf ("x = %d, y = %d, Angle = %d\n", x, y, deg);

	x = 3; y = 3; deg = 0;
	deg = GetAngle(x, y);
	printf ("x = %d, y = %d, Angle = %d\n", x, y, deg);

}
#endif
