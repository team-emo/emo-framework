#include <math.h>

#define MATH_PI       3.14159265358979323846
#define MATH_PI_2     1.57079632679489661923
#define MATH_PI_4     0.785398163397448309616
#define MATH_1_PI     0.318309886183790671538
#define MATH_2_PI     0.636619772367581343076
#define MATH_TORAD    (float)(MATH_PI/180.0)
#define MATH_TODEG    (float)(180.0/MATH_PI)
#define TORAD(x) (float)(x*MATH_TORAD)
#define TODEG(x) (float)(x*MATH_TODEG)

bool isPowerOfTwo(int x) {
	return (x != 0) && ((x & (x - 1)) == 0);
}

int nextPowerOfTwo(int minimum) {
	if(isPowerOfTwo(minimum)) {
		return minimum;
	}
	int i = 0;
	while(true) {
		i++;
		if(pow(2, i) >= minimum) {
			return (int)pow(2, i);
		}
	}
}
