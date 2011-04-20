#ifndef H_EMO_OPERATOR
#define H_EMO_OPERATOR
struct char_comparator {
    bool operator()(const char* s1, const char* s2) const {
        return strcmp(s1, s2) == 0;
    }
};
#endif

bool isPowerOfTwo(int x);
int nextPowerOfTwo(int minimum);

