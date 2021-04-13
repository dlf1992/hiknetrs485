#********************************************
#两种摄像机类型
#GP----光珀摄像机
#OB----奥比摄像机
#********************************************
#TM_CAMERA_TYPE=GP

DEBUG_DIR=./obj
SRC_DIR= ./src
INC_DIR=./include

SRC=$(wildcard $(SRC_DIR)/*.cpp )
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(DEBUG_DIR)/%.o,$(SRC))


TARGET=netrs485

INSTALL_PATH ?= .

ARCH ?=

CC=$(ARCH)gcc
CPP=$(ARCH)g++
AR=$(ARCH)ar
STRIP=$(ARCH)strip


CFLAGS += -Wall


CFLAGS  += $(foreach dir,$(INC_DIR),-I$(dir))
LDFLAGS += -lpthread

all:$(TARGET)
$(TARGET): $(OBJS)
	$(CPP) $(OBJS) -o $@ $(CFLAGS) $(LDFLAGS)
	$(STRIP) $(TARGET)



$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@ 

print:
	@echo $(SRC)
	@echo $(OBJS)

	
clean:
	-rm $(OBJS) $(LIB_TARGET)

