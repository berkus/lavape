################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Lava_UI/LavaMainFrameBase.cpp \
../src/Lava_UI/Lava_UI_all.cpp 

OBJS += \
./src/Lava_UI/LavaMainFrameBase.o \
./src/Lava_UI/Lava_UI_all.o 

CPP_DEPS += \
./src/Lava_UI/LavaMainFrameBase.d \
./src/Lava_UI/Lava_UI_all.d 


# Each subdirectory must supply rules for building sources it contributes
src/Lava_UI/%.o: ../src/Lava_UI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


