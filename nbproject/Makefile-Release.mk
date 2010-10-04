#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/ChordWidget.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/EventGUI.o \
	${OBJECTDIR}/src/Chord.o \
	${OBJECTDIR}/src/ActionGUI.o \
	${OBJECTDIR}/src/Sequencer.o \
	${OBJECTDIR}/src/messages.o \
	${OBJECTDIR}/src/Action.o \
	${OBJECTDIR}/src/MainWindow.o \
	${OBJECTDIR}/src/TreeModels.o \
	${OBJECTDIR}/src/EventsWindow.o \
	${OBJECTDIR}/src/MidiDriver.o \
	${OBJECTDIR}/src/Files.o \
	${OBJECTDIR}/src/Event.o \
	${OBJECTDIR}/src/SequencerGUI.o \
	${OBJECTDIR}/src/global.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=`pkg-config --libs --cflags gtkmm-2.4` 
CXXFLAGS=`pkg-config --libs --cflags gtkmm-2.4` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Release.mk build/Release/GNU-Linux-x86/tests/TestFiles/f1

build/Release/GNU-Linux-x86/tests/TestFiles/f1: ${OBJECTFILES}
	${MKDIR} -p build/Release/GNU-Linux-x86/tests/TestFiles
	${LINK.cc} -lasound -o ${TESTDIR}/TestFiles/f1 -s ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/ChordWidget.o: src/ChordWidget.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ChordWidget.o src/ChordWidget.cpp

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/EventGUI.o: src/EventGUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/EventGUI.o src/EventGUI.cpp

${OBJECTDIR}/src/Chord.o: src/Chord.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Chord.o src/Chord.cpp

${OBJECTDIR}/src/ActionGUI.o: src/ActionGUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ActionGUI.o src/ActionGUI.cpp

${OBJECTDIR}/src/Sequencer.o: src/Sequencer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Sequencer.o src/Sequencer.cpp

${OBJECTDIR}/src/messages.o: src/messages.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/messages.o src/messages.cpp

${OBJECTDIR}/src/Action.o: src/Action.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Action.o src/Action.cpp

${OBJECTDIR}/src/MainWindow.o: src/MainWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/MainWindow.o src/MainWindow.cpp

${OBJECTDIR}/src/TreeModels.o: src/TreeModels.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/TreeModels.o src/TreeModels.cpp

${OBJECTDIR}/src/EventsWindow.o: src/EventsWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/EventsWindow.o src/EventsWindow.cpp

${OBJECTDIR}/src/MidiDriver.o: src/MidiDriver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/MidiDriver.o src/MidiDriver.cpp

${OBJECTDIR}/src/Files.o: src/Files.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Files.o src/Files.cpp

${OBJECTDIR}/src/Event.o: src/Event.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Event.o src/Event.cpp

${OBJECTDIR}/src/SequencerGUI.o: src/SequencerGUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/SequencerGUI.o src/SequencerGUI.cpp

${OBJECTDIR}/src/global.o: src/global.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -O3 -s -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/global.o src/global.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} build/Release/GNU-Linux-x86/tests/TestFiles/f1

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
