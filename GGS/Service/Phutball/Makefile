# Makefile.gnu [bof] (c) Igor Durdanovic

MAIN = Phutball

CC_INCS = -I../GameLib/src
LD_ARGS = -L../GameLib/lib
LD_LIBS = -lGGS_GameLib

include ../../config/opts.c++

CC_DEFS = -DLOG=2 -DLOG_ERR -DSERVICE $(LIB_DEF)
CC_ARGS = $(CC_FLAGS) $(CC_DEFS) $(CC_INCS)

include ../../config/opts.bin.rules

# Makefile.gnu [eof] (c) Igor Durdanovic
