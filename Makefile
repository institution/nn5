# Makefile
# $@ -- The file name of the target of the rule.
# $* -- The stem with which an implicit rule matches
# $< -- The name of the first prerequisite.
# $^ -- The names of all the prerequisites
# $(VARS:%.cpp=%.o) -- pattern replace

#CC:=g++
CC:=clang++
#CC:=emcc

# output files
OUTS:=ai/test_network ai/main ai/mtest ai/test misc/speed_test ext/test go/test front/main gui/main

# targets
debug: ai/test_network ai/main ai/test misc/speed_test ext/test go/test front/main gui/main
release: ai/test_network ai/main ai/test misc/speed_test ext/test go/test front/main gui/main

# em opts
EMOPTS:= 
EMOPTS+=-s WASM=1
#EMOPTS+=-s USE_SDL=2 
EMOPTS+=-s DISABLE_EXCEPTION_CATCHING=0 
#EMOPTS+=-s SAFE_HEAP=1 
EMOPTS+=-s ASSERTIONS=1 
#EMOPTS+=-s DEMANGLE_SUPPORT=1
#EMOPTS+=-s EXCEPTION_DEBUG=1 

# compiler options
CCOPTS:=
CCOPTS+=-std=c++17
#CCOPTS+=-stdlib=libc++
CCOPTS+=-I./src
CCOPTS+=-Wsign-compare -Wreturn-type -Wparentheses -Wpedantic -Wconversion-null 
#CCOPTS+=-fmax-errors=1
#-Wno-vla-extension
CCOPTS+=-ferror-limit=5


# linker options
LLOPTS:=


# emcc
ifeq (${CC}, emcc)	
	CCOPTS+=${EMOPTS}
	LLOPTS+=${EMOPTS}
	OUT_EXT:=.html	
	#LLOPTS+=--preload-file res	
else
	LLOPTS+=-L./lib 
	LLOPTS+=-lSDL2
	LLOPTS+=-lGL -lGLEW 
	LLOPTS+=-lpthread
	#LLOPTS+=-lfmt
	OUT_EXT:=
endif



debug:   CCOPTS+=-O0 -g
debug:   LLOPTS+=-O0 -g
release: CCOPTS+=-O3 
release: LLOPTS+=-O3 






# assert dirs
$(shell mkdir -p obj)
$(shell find src/ -type d | cut -c 5- | xargs -I{} mkdir -p obj/{})
$(shell mkdir -p bin)
$(shell find src/ -type d | cut -c 5- | xargs -I{} mkdir -p bin/{})

# list of compiled source obj/fname.cpp.obj
OBJS:=$(shell find src -name '*.cpp')
OBJS:=$(OBJS:src/%.cpp=obj/%.cpp.obj)

-include $(OBJS:%.obj=%.d)


# compiler
${OBJS}: obj/%.obj: src/%
	${CC} -c -MMD -MP -o $@ $< ${CCOPTS}
	
# linker
${OUTS}: $(OBJS)
	${CC} -o bin/$@${OUT_EXT} obj/$@.cpp.obj  $(filter-out $(OUTS:%=obj/%.cpp.obj),$(OBJS)) ${LLOPTS}
	


clean:
	rm -rf obj/* bin/* core*
	




