SQUIRREL_SRC_FILES += squirrel/squirrel/sqapi.cpp \
	squirrel/squirrel/sqbaselib.cpp \
	squirrel/squirrel/sqfuncstate.cpp \
	squirrel/squirrel/sqdebug.cpp \
	squirrel/squirrel/sqlexer.cpp \
	squirrel/squirrel/sqobject.cpp \
	squirrel/squirrel/sqcompiler.cpp \
	squirrel/squirrel/sqstate.cpp \
	squirrel/squirrel/sqtable.cpp \
	squirrel/squirrel/sqmem.cpp \
	squirrel/squirrel/sqvm.cpp \
	squirrel/squirrel/sqclass.cpp \
	squirrel/sqstdlib/sqstdblob.cpp \
	squirrel/sqstdlib/sqstdio.cpp \
	squirrel/sqstdlib/sqstdstream.cpp \
	squirrel/sqstdlib/sqstdmath.cpp \
	squirrel/sqstdlib/sqstdsystem.cpp \
	squirrel/sqstdlib/sqstdstring.cpp \
	squirrel/sqstdlib/sqstdaux.cpp \
	squirrel/sqstdlib/sqstdrex.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/squirrel/include
