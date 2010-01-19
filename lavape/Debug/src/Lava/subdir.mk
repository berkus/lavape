################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Lava/Lava.cpp \
../src/Lava/LavaDoc.cpp \
../src/Lava/Lava_all.cpp \
../src/Lava/MainFrm.cpp 

OBJS += \
./src/Lava/Lava.o \
./src/Lava/LavaDoc.o \
./src/Lava/Lava_all.o \
./src/Lava/MainFrm.o 

CPP_DEPS += \
./src/Lava/Lava.d \
./src/Lava/LavaDoc.d \
./src/Lava/Lava_all.d \
./src/Lava/MainFrm.d 


# Each subdirectory must supply rules for building sources it contributes
src/Lava/%.o: ../src/Lava/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


