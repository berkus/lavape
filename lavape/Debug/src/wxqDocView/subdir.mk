################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/wxqDocView/docview.o \
../src/wxqDocView/mdiframes.o \
../src/wxqDocView/moc_docview.o \
../src/wxqDocView/moc_mdiframes.o 

CPP_SRCS += \
../src/wxqDocView/docview.cpp \
../src/wxqDocView/mdiframes.cpp \
../src/wxqDocView/moc_docview.cpp \
../src/wxqDocView/moc_mdiframes.cpp \
../src/wxqDocView/wxqDocView_all.cpp 

OBJS += \
./src/wxqDocView/docview.o \
./src/wxqDocView/mdiframes.o \
./src/wxqDocView/moc_docview.o \
./src/wxqDocView/moc_mdiframes.o \
./src/wxqDocView/wxqDocView_all.o 

CPP_DEPS += \
./src/wxqDocView/docview.d \
./src/wxqDocView/mdiframes.d \
./src/wxqDocView/moc_docview.d \
./src/wxqDocView/moc_mdiframes.d \
./src/wxqDocView/wxqDocView_all.d 


# Each subdirectory must supply rules for building sources it contributes
src/wxqDocView/%.o: ../src/wxqDocView/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


