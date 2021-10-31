# A generic Makefile for building any project
# with the following directory structure.
# ./src
# ./obj
# ./bin
# ./src/MTracker.o ./src/Tracker.o ./src/Server.o

# debug: for printing any variable
print-%  : ; @echo $* = $($*)


#############
# Variables #
#############
CC      = g++
CCFLAGS = -Wall -std=c++1z
LDFLAGS	= -lcrypto -lpthread
INCLUDE	= -I./ -I/usr/local/opt/openssl/include/
SRCDIR  = ./src
OBJDIR 	= ./obj
BINDIR  = ./bin

TARGET_NODE	= node
TARGET_TRACKER	= tracker


######################################
# creating objects for build targets #
######################################

FILES   := $(wildcard $(SRCDIR)/*.C)
OBJECTS := $(FILES:$(SRCDIR)/%.C=$(OBJDIR)/%.o)

FILES_TRACKER 	:= $(SRCDIR)/MTracker.C $(SRCDIR)/Tracker.C $(SRCDIR)/Server.C
OBJECTS_TRACKER	:= $(FILES_TRACKER:$(SRCDIR)/%.C=$(OBJDIR)/%.o)

FILES_NODE 	:= $(SRCDIR)/MNode.C $(SRCDIR)/Node.C $(SRCDIR)/Client.C $(SRCDIR)/Server.C
OBJECTS_NODE	:= $(FILES_NODE:$(SRCDIR)/%.C=$(OBJDIR)/%.o)


##############################
# Rules building the targets #
##############################

all: $(TARGET_TRACKER) $(TARGET_NODE)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.C
	$(CC) $(CCFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET_TRACKER):$(OBJECTS_TRACKER)
	$(CC) $(OBJECTS_TRACKER) -o $@
	mv $(TARGET_TRACKER) $(BINDIR)
	@echo "Tracker Built Successfully."


$(TARGET_NODE):$(OBJECTS_NODE)
	$(CC) $(LDFLAGS) $(OBJECTS_NODE) -o $@
	mv $(TARGET_NODE) $(BINDIR)
	@echo "Node Built Successfully."


#################
# Cleanup rules #
#################

clean:	clean_tracker clean_node

clean_tracker:
	@echo "Cleaning all the object files and binaries of \"tracker\"."
	rm -f core
	rm -f $(OBJECTS_TRACKER) $(BINDIR)/$(TARGET_TRACKER) $(TARGET_TRACKER)

clean_node:
	@echo "Cleaning all the object files and binaries of \"node\"."
	rm -f core
	rm -f $(OBJECTS_NODE) $(BINDIR)/$(TARGET_NODE) $(TARGET_NODE)




