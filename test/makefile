TARGET?=main
SHELL=bash
SHELL?=bash
CXX=clang++
CXX?=clang++
CXX_FLAGS=-std=c++11 -fpic
CXX_EXTRA?=
CTAGS_I_PATH?=./
LD_FLAGS=
EXTRA_LD_FLAGS?=
ADD_SANITIZERS_CC= -g -fsanitize=address -fno-omit-frame-pointer
ADD_SANITIZERS_LD= -g -fsanitize=address
MEM_SANITIZERS_CC= -g -fsanitize=memory -fno-omit-frame-pointer
MEM_SANITIZERS_LD= -g -fsanitize=memory
UB_SANITIZERS_CC= -g -fsanitize=undefined -fno-omit-frame-pointer
UB_SANITIZERS_LD= -g -fsanitize=undefined
COV_CXX= -fprofile-instr-generate -fcoverage-mapping
COV_LD= -fprofile-instr-generate
# BUILD_MODES are=RELEASE(default), DEBUG,ADDSAN,MEMSAN,UBSAN
BUILD_MODE?=RELEASE
OBJ_LIST:=$(patsubst %.cpp, %.o, $(wildcard *.cpp))
ASM_LIST:=$(patsubst %.cpp, %.dis, $(wildcard *.cpp))
WASM_LIST:=$(patsubst %.cpp, %.wasm, $(wildcard *.cpp))
JS_LIST:=$(patsubst %.cpp, %.js, $(wildcard *.cpp))

ifeq ($(BUILD_MODE), ADDSAN)
ifeq ($(CXX), g++)
$(error This build mode is only useable with clang++.)
endif
CXX_EXTRA+=$(ADD_SANITIZERS_CC)
EXTRA_LD_FLAGS+=$(ADD_SANITIZERS_LD)
endif

ifeq ($(BUILD_MODE), MEMSAN)
ifeq ($(CXX), g++)
$(error This build mode is only useable with clang++.)
endif
CXX_EXTRA+=$(MEM_SANITIZERS_CC)
EXTRA_LD_FLAGS+=$(MEM_SANITIZERS_LD)
endif

ifeq ($(BUILD_MODE), UBSAN)
ifeq ($(CXX), g++)
$(error This build mode is only useable with clang++.)
endif
CXX_EXTRA+=$(UB_SANITIZERS_CC)
EXTRA_LD_FLAGS+=$(UB_SANITIZERS_LD)
endif

SRCS:=$(wildcard *.cpp)
HDRS:=$(wildcard *.h)
CXX_FLAGS+=$(CXX_EXTRA)
LD_FLAGS+=$(EXTRA_LD_FLAGS)

.DEFAULT:all

.PHONY:all clean help ASM SO TAGS WASM JS

all:$(TARGET)

everything:$(TARGET) A ASM SO $(TARGET)-static $(TARGET)-dbg TAGS $(TARGET)-cov WASM JS

depend:.depend

.depend:$(SRCS)
	rm -rf .depend
	$(CXX) -MM $(CXX_FLAGS) $^ > ./.depend
	echo $(patsubst %.o:, %.odbg:, $(shell $(CXX) -MM $(CXX_FLAGS) $^)) | sed -r 's/[A-Za-z0-9\-\_]+\.odbg/\n&/g' >> ./.depend
	echo $(patsubst %.o:, %.ocov:, $(shell $(CXX) -MM $(CXX_FLAGS) $^)) | sed -r 's/[A-Za-z0-9\-\_]+\.ocov/\n&/g' >> ./.depend

-include ./.depend

.cpp.o:
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%.odbg:%.cpp
	$(CXX) $(CXX_FLAGS) -g -c $< -o $@

%.ocov:%.cpp
	$(CXX) $(CXX_FLAGS) $(COV_CXX) -c $< -o $@

$(TARGET): $(TARGET).o
	$(CXX) $(LD_FLAGS) $^ -o $@

$(TARGET)-static: $(TARGET).o
	$(CXX) $(LD_FLAGS) $^ -static -o $@

$(TARGET)-dbg: $(TARGET).odbg
	$(CXX) $(LD_FLAGS) $^ -g -o $@

$(TARGET)-cov: $(TARGET).ocov
	$(CXX) $(LD_FLAGS) $^ $(COV_LD) -o $@

cov: runcov
	@llvm-profdata merge -sparse ./default.profraw -o ./default.profdata
	@llvm-cov show $(TARGET)-cov -instr-profile=default.profdata

covrep: runcov
	@llvm-profdata merge -sparse ./default.profraw -o ./default.profdata
	@llvm-cov report $(TARGET)-cov -instr-profile=default.profdata

ASM:$(ASM_LIST)

SO:$(TARGET).so

A:$(TARGET).a

WASM:$(WASM_LIST)

JS:$(JS_LIST)

TAGS:tags

#https://github.com/rizsotto/Bear
BEAR: clean
	bear make

tags:$(SRCS)
	$(shell $(CXX) -c -I $(CTAGS_I_PATH) -M $(SRCS)|\
		sed -e 's/[\\ ]/\n/g'|sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d'|\
		ctags -L - --c++-kinds=+p --fields=+iaS --extra=+q)

%.dis: %.o
	objdump -r -d -M intel -S $< > $@

%.wasm: %.cpp
	$(CXX) --compile $< --target=wasm32-unknown-unknown-wasm --output $@

%.js: %.cpp
	em++ $< -o $@

$(TARGET).so: $(TARGET).o
	$(CXX) $(LD_FLAGS) $^ -shared -o $@

$(TARGET).a: $(TARGET).o
	ar rcs $(TARGET).a $(TARGET).o

runcov: $(TARGET)-cov
	$(TARGET)-cov

test: $(TARGET)
	$(TARGET)

valgrind: $(TARGET)
	- valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all $(TARGET)

format:
	- clang-format -i $(SRCS) $(HDRS)

clean:
	rm -f *.o *.dis *.odbg *.ocov *.js *~ $(TARGET) $(TARGET).so $(TARGET)-static $(TARGET)-dbg $(TARGET).a $(TARGET)-cov

deepclean: clean
	- rm tags
	- rm .depend
	- rm ./default.profraw ./default.profdata
	- rm vgcore.*
	- rm compile_commands.json

help:
	@echo "--all is the default target, runs $(TARGET) target"
	@echo "--everything will build everything"
	@echo "--SO will generate the so"
	@echo "--ASM will generate assembly files"
	@echo "--TAGS will generate tags file"
	@echo "--$(TARGET) builds the dynamically-linked executable"
	@echo "--$(TARGET)-dbg will generate the debug build. BUILD_MODE should be set to DEBUG to work"
	@echo "--$(TARGET)-static will statically link the executable to the libraries"
	@echo "--$(TARGET)-cov is the coverage build"
	@echo "--cov will print the coverage report"
	@echo "--covrep will print the line coverage report"
	@echo "--A will build the static library"
	@echo "--TAGS will build the tags file"
	@echo "--clean"
	@echo "--deepclean will clean almost everything"
