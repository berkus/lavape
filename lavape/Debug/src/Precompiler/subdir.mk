################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/Precompiler/Args.o \
../src/Precompiler/Atoms.o \
../src/Precompiler/CdpCoder.o \
../src/Precompiler/CmdError.o \
../src/Precompiler/CmdParms.o \
../src/Precompiler/Coder.o \
../src/Precompiler/ContxtCK.o \
../src/Precompiler/DParser.o \
../src/Precompiler/ElemCode.o \
../src/Precompiler/FioCoder.o \
../src/Precompiler/IntCode.o \
../src/Precompiler/LexScanner.o \
../src/Precompiler/PC.o \
../src/Precompiler/PCerrors.o \
../src/Precompiler/Parser.o \
../src/Precompiler/ProCoder.o \
../src/Precompiler/SigCoder.o \
../src/Precompiler/TypCoder.o \
../src/Precompiler/XParser.o 

CPP_SRCS += \
../src/Precompiler/Args.cpp \
../src/Precompiler/Atoms.cpp \
../src/Precompiler/CdpCoder.cpp \
../src/Precompiler/CmdError.cpp \
../src/Precompiler/CmdParms.cpp \
../src/Precompiler/Coder.cpp \
../src/Precompiler/ContxtCK.cpp \
../src/Precompiler/DParser.cpp \
../src/Precompiler/ElemCode.cpp \
../src/Precompiler/FioCoder.cpp \
../src/Precompiler/IntCode.cpp \
../src/Precompiler/LexScanner.cpp \
../src/Precompiler/PC.cpp \
../src/Precompiler/PCerrors.cpp \
../src/Precompiler/Parser.cpp \
../src/Precompiler/Precompiler_all.cpp \
../src/Precompiler/ProCoder.cpp \
../src/Precompiler/SigCoder.cpp \
../src/Precompiler/TypCoder.cpp \
../src/Precompiler/XParser.cpp 

OBJS += \
./src/Precompiler/Args.o \
./src/Precompiler/Atoms.o \
./src/Precompiler/CdpCoder.o \
./src/Precompiler/CmdError.o \
./src/Precompiler/CmdParms.o \
./src/Precompiler/Coder.o \
./src/Precompiler/ContxtCK.o \
./src/Precompiler/DParser.o \
./src/Precompiler/ElemCode.o \
./src/Precompiler/FioCoder.o \
./src/Precompiler/IntCode.o \
./src/Precompiler/LexScanner.o \
./src/Precompiler/PC.o \
./src/Precompiler/PCerrors.o \
./src/Precompiler/Parser.o \
./src/Precompiler/Precompiler_all.o \
./src/Precompiler/ProCoder.o \
./src/Precompiler/SigCoder.o \
./src/Precompiler/TypCoder.o \
./src/Precompiler/XParser.o 

CPP_DEPS += \
./src/Precompiler/Args.d \
./src/Precompiler/Atoms.d \
./src/Precompiler/CdpCoder.d \
./src/Precompiler/CmdError.d \
./src/Precompiler/CmdParms.d \
./src/Precompiler/Coder.d \
./src/Precompiler/ContxtCK.d \
./src/Precompiler/DParser.d \
./src/Precompiler/ElemCode.d \
./src/Precompiler/FioCoder.d \
./src/Precompiler/IntCode.d \
./src/Precompiler/LexScanner.d \
./src/Precompiler/PC.d \
./src/Precompiler/PCerrors.d \
./src/Precompiler/Parser.d \
./src/Precompiler/Precompiler_all.d \
./src/Precompiler/ProCoder.d \
./src/Precompiler/SigCoder.d \
./src/Precompiler/TypCoder.d \
./src/Precompiler/XParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/Precompiler/%.o: ../src/Precompiler/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


