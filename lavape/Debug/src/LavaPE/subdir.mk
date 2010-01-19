################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/LavaPE/Bars.cpp \
../src/LavaPE/Boxes.cpp \
../src/LavaPE/DebuggerPE.cpp \
../src/LavaPE/ExecTree.cpp \
../src/LavaPE/FindRefsBox.cpp \
../src/LavaPE/GenHTML.cpp \
../src/LavaPE/InclView.cpp \
../src/LavaPE/LavaPE.cpp \
../src/LavaPE/LavaPEDoc.cpp \
../src/LavaPE/LavaPEFrames.cpp \
../src/LavaPE/LavaPEStringInit.cpp \
../src/LavaPE/LavaPEView.cpp \
../src/LavaPE/LavaPEWizard.cpp \
../src/LavaPE/LavaPE_all.cpp \
../src/LavaPE/TreeView.cpp \
../src/LavaPE/VTView.cpp 

OBJS += \
./src/LavaPE/Bars.o \
./src/LavaPE/Boxes.o \
./src/LavaPE/DebuggerPE.o \
./src/LavaPE/ExecTree.o \
./src/LavaPE/FindRefsBox.o \
./src/LavaPE/GenHTML.o \
./src/LavaPE/InclView.o \
./src/LavaPE/LavaPE.o \
./src/LavaPE/LavaPEDoc.o \
./src/LavaPE/LavaPEFrames.o \
./src/LavaPE/LavaPEStringInit.o \
./src/LavaPE/LavaPEView.o \
./src/LavaPE/LavaPEWizard.o \
./src/LavaPE/LavaPE_all.o \
./src/LavaPE/TreeView.o \
./src/LavaPE/VTView.o 

CPP_DEPS += \
./src/LavaPE/Bars.d \
./src/LavaPE/Boxes.d \
./src/LavaPE/DebuggerPE.d \
./src/LavaPE/ExecTree.d \
./src/LavaPE/FindRefsBox.d \
./src/LavaPE/GenHTML.d \
./src/LavaPE/InclView.d \
./src/LavaPE/LavaPE.d \
./src/LavaPE/LavaPEDoc.d \
./src/LavaPE/LavaPEFrames.d \
./src/LavaPE/LavaPEStringInit.d \
./src/LavaPE/LavaPEView.d \
./src/LavaPE/LavaPEWizard.d \
./src/LavaPE/LavaPE_all.d \
./src/LavaPE/TreeView.d \
./src/LavaPE/VTView.d 


# Each subdirectory must supply rules for building sources it contributes
src/LavaPE/%.o: ../src/LavaPE/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


