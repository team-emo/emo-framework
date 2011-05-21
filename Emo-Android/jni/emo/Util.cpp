#include <math.h>

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

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}
