# Made By YDJ
#

CROSS_COMPILE	:=
CC				:=$(CROSS_COMPILE)gcc
LD				:=ld
INCLUDES		:=-I. -I$(TOPDIR)/include -I$(TOPDIR)/common
DEFINES			:=#-DDEBUG
CFLAGS			:=-g -W -Wall $(INCLUDES) $(DEFINES)

#############################################################
.SUFFIXES:.o .c .S

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

#############################################################
