# This makefile is intended for use with GNU make
# Made by YDJ

ifndef TOPDIR
TOPDIR	:= ../..
endif

include $(TOPDIR)/Configs.mk

TARGET	:= built-in.o
SUBDIRS	:= 
OBJS	:= $(patsubst %.S, %.o, $(wildcard *.S))
OBJS	+= $(patsubst %.c, %.o, $(wildcard *.c))

all : compile $(OBJS)
	$(LD) -r $(addsuffix /$(TARGET), $(SUBDIRS)) $(OBJS) -o $(TARGET)

include $(TOPDIR)/Rules.mk
