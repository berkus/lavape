################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/LavaPE_UI/LavaPEMainFrameBase.cpp \
../src/LavaPE_UI/LavaPE_UI_all.cpp 

OBJS += \
./src/LavaPE_UI/LavaPEMainFrameBase.o \
./src/LavaPE_UI/LavaPE_UI_all.o 

CPP_DEPS += \
./src/LavaPE_UI/LavaPEMainFrameBase.d \
./src/LavaPE_UI/LavaPE_UI_all.d 


# Each subdirectory must supply rules for building sources it contributes
src/LavaPE_UI/%.o: ../src/LavaPE_UI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


