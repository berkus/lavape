################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Interpreter/ConstructsX.cpp \
../src/Interpreter/Debugger.cpp \
../src/Interpreter/Interpreter_all.cpp \
../src/Interpreter/LavaProgram.cpp 

OBJS += \
./src/Interpreter/ConstructsX.o \
./src/Interpreter/Debugger.o \
./src/Interpreter/Interpreter_all.o \
./src/Interpreter/LavaProgram.o 

CPP_DEPS += \
./src/Interpreter/ConstructsX.d \
./src/Interpreter/Debugger.d \
./src/Interpreter/Interpreter_all.d \
./src/Interpreter/LavaProgram.d 


# Each subdirectory must supply rules for building sources it contributes
src/Interpreter/%.o: ../src/Interpreter/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


