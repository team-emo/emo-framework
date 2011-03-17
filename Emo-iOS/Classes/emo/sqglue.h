bool initSQGlue();
bool stopSQGlue();
bool sqGlue_compile(const char* script, const char* sourcename);
bool callSqGlueFunctionNoParam(const char* name);
bool callSqGlueFunctionString(const char* name, const char* param);
int  callSqGlueFunctionNoParam_Int(const char* name, int defaultValue);
bool callSqGlueFunctionNoParam_Bool(const char* name, bool defaultValue);
int  callSqGlueFunctionInt_Int(const char* name, int param, int defaultValue);

