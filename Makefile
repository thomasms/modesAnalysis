# ROOT headers and libraries
ROOT_INCLUDES  = -I$(shell root-config --incdir)
ROOT_LIBRARIES = $(shell root-config --glibs) -lMinuit -lGeom -lEG -lEGPythia6

INCLUDES := $(ROOT_INCLUDES) $(LINUX_SYS_INCLUDES)

#LIBS = $(ROOT_LIBRARIES)

#local src and include directories for my own created cxx and header files
IDIR    = include
SDIR    = src
ODIR    = obj

VPATH   = $(SDIR) $(IDIR) 

#compiler
CC=clang
CXX=clang++
#LD=clang++

RM=rm -f

CFLAGS = -c -g -std=c++11 -I$(IDIR) -I$(shell root-config --incdir)
LDFLAGS = -g -Wall
LDLIBS = $(shell root-config --glibs) -lSpectrum

TARGET = run

all: $(ODIR) $(TARGET)

SOURCE1 = $(TARGET)
SOURCE2 = Plotter
SOURCE3 = DataCardsRecord
SOURCE4 = DataCards
SOURCE5 = Manager
SOURCE6 = Handler
SOURCE7 = EfficiencyPlot
SOURCE8 = PurityPlot
SOURCE9 = BasePlot
SOURCE10 = FOMPlot
SOURCE11 = GaussianFitter
SOURCE12 = Utils

OBJ = $(SOURCE1).o $(SOURCE2).o $(SOURCE3).o $(SOURCE4).o $(SOURCE5).o $(SOURCE6).o $(SOURCE7).o $(SOURCE8).o $(SOURCE9).o $(SOURCE10).o $(SOURCE11).o
OBJ += $(SOURCE12).o
OBJS = $(addprefix $(ODIR)/,$(OBJ))

#define root system location
ROOTSYS=$(shell root-config --incdir)

INC = $(IDIR)/*.hh $(ROOTSYS)/*.h

$(ODIR):
	mkdir $(ODIR)

#executable file linking always comes first!
$(TARGET): $(OBJS)
	@echo ........Compiling 
	$(CXX) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $(TARGET)
	@echo Done!

#compile the libraries
$(ODIR)/%.o: %.cc $(INC)
	$(CXX) $(CFLAGS) $(LIBS) $< -o $@

clean:
	$(RM) $(OBJS)
	@rm -rf $(ODIR)	
	$(RM) $(TARGET)


