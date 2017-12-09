
# Configurations #

PROJECTNAME = werewolf
TARGET = lib$(PROJECTNAME).so

SOURCEDIR = \
	src

KARUTADIR ?= ../..

INCLUDEPATH = $(SOURCEDIR) $(KARUTADIR)/src/core $(KARUTADIR)/src/network $(KARUTADIR)/src/util

SOURCES = $(foreach dir,$(SOURCEDIR),$(wildcard $(dir)/*.cpp))

CPPFLAGS = -std=c++11 -fexceptions -Wall

BUILDCONFIG = release

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CPPFLAGS += -g
	BUILDCONFIG = debug
endif

BINDIR = bin/$(BUILDCONFIG)
OBJDIR = obj/$(BUILDCONFIG)

EXECFLAGS = -L"$(KARUTADIR)/$(BINDIR)" -lkaruta -Wl,-rpath='$$ORIGIN'

# Build Target #

OBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

vpath %.cpp $(INCLUDEPATH)
vpath %.h $(INCLUDEPATH)

CCPARAM = $(CPPFLAGS) $(addprefix -I,$(INCLUDEPATH))

$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) -shared -o $(BINDIR)/$(TARGET) $(OBJECTS) $(LFLAGS) $(EXECFLAGS)
	cp -f $(BINDIR)/$(TARGET) $(KARUTADIR)/bin/$(BUILDCONFIG)

.SECONDEXPANSION:
$(OBJECTS): $$(addsuffix .cpp,$$(basename $$(notdir $$@))) | $(OBJDIR)
	$(CXX) -o $@ -c $(filter %/$(addsuffix .cpp,$(basename $(notdir $@))),$(SOURCES)) $(CCPARAM) -fPIC

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

# Build Unit Test #

TESTUNITS = $(wildcard test/*.cpp)
TESTPREPS = $(addprefix $(OBJDIR)/prep/unittest/, $(addsuffix .cpp, $(basename $(notdir $(TESTUNITS)))))
TESTOBJECTS = $(addprefix $(OBJDIR)/unittest/, $(addsuffix .o, $(basename $(notdir $(TESTUNITS)))))

test: $(KARUTADIR)/$(BINDIR)/$(PROJECTNAME)-unittest
	./$(KARUTADIR)/$(BINDIR)/$(PROJECTNAME)-unittest

$(KARUTADIR)/$(BINDIR)/$(PROJECTNAME)-unittest: $(BINDIR)/unittest
	cp -f $(BINDIR)/unittest $(KARUTADIR)/$(BINDIR)/$(PROJECTNAME)-unittest

$(BINDIR)/unittest: $(TESTOBJECTS) | $(BINDIR)/$(TARGET)
	$(CXX) -o $(BINDIR)/unittest $(TESTOBJECTS) $(LFLAGS) $(EXECFLAGS) -l$(PROJECTNAME)

$(TESTOBJECTS): $$(addprefix $(OBJDIR)/prep/unittest/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) | $(OBJDIR)/unittest
	$(CXX) -o $@ -c $(addprefix $(OBJDIR)/prep/unittest/, $(addsuffix .cpp, $(basename $(notdir $@)))) $(CCPARAM) -Itest

$(TESTPREPS): $$(addprefix test/, $$(addsuffix .cpp, $$(basename $$(notdir $$@)))) $(OBJDIR)/prep/unittest
	./test/prep.sh test/$(notdir $@) > $@

$(OBJDIR)/unittest:
	mkdir -p $(OBJDIR)/unittest

$(OBJDIR)/prep/unittest:
	chmod +x test/prep.sh
	mkdir -p $(OBJDIR)/prep/unittest
