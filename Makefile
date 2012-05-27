# This makefile is intended for use with GNU make
# Made by YDJ

.EXPORT_ALL_VARIABLES:

TARGET	:=ARMISS
TOPDIR	:=$(shell /bin/pwd)
SUBDIRS	:= common thumb main 

########################################################

include $(TOPDIR)/Configs.mk


all: compile $(OBJS)
	$(CC) $(OBJS) $(addsuffix /built-in.o, $(SUBDIRS)) -o $(TARGET)

echo :
	@echo $(TOPDIR)

include $(TOPDIR)/Rules.mk

