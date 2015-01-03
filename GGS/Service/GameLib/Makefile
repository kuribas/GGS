# (C) Igor Durdanovic -- [bof] GameLib/Makefile
#
# Description:
#
# 1) for all X in .src/X.C creates dependencies .mkf/X.m
# 2) appends compilation instruction for .obj/X.o to .mkf/X.m
# 3) creates library instructions for .lib/lib(NAME).so to .mkf/Makefile.mak 
# 4) appends all .mkf/X.m to .mkf/Makefile.mak
# 5) makes .mkf/makefile.mak
#
# the only customizable file is 

NAME = GGS_GameLib

include ../../config/opts.c++

CC_DEFS = -DLOG=2 -DLOG_ERR -DSERVICE $(LIB_DEF)
CC_ARGS = $(CC_FLAGS) $(CC_DEFS) $(CC_INCS)

include ../../config/opts.lib.rules

# (C) Igor Durdanovic -- [eof] GameLib/Makefile
