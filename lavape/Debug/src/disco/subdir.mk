################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/disco/ASN1.o \
../src/disco/ASN1File.o \
../src/disco/ASN1pp.o \
../src/disco/AnyType.o \
../src/disco/CDPpp.o \
../src/disco/CHAINANY.o \
../src/disco/ChString.o \
../src/disco/ChainA.o \
../src/disco/Convert.o \
../src/disco/DIO.o \
../src/disco/DString.o \
../src/disco/DateTime.o \
../src/disco/Halt.o \
../src/disco/InFile.o \
../src/disco/MachDep.o \
../src/disco/NESTANY.o \
../src/disco/NestA.o \
../src/disco/OutFile.o \
../src/disco/PMDump.o \
../src/disco/SET.o \
../src/disco/SETpp.o \
../src/disco/STR.o \
../src/disco/wb_obj.o 

CPP_SRCS += \
../src/disco/ASN1.cpp \
../src/disco/ASN1File.cpp \
../src/disco/ASN1pp.cpp \
../src/disco/AnyType.cpp \
../src/disco/CDPpp.cpp \
../src/disco/CHAINANY.cpp \
../src/disco/ChString.cpp \
../src/disco/ChainA.cpp \
../src/disco/Convert.cpp \
../src/disco/DIO.cpp \
../src/disco/DString.cpp \
../src/disco/DateTime.cpp \
../src/disco/Halt.cpp \
../src/disco/InFile.cpp \
../src/disco/MachDep.cpp \
../src/disco/NESTANY.cpp \
../src/disco/NestA.cpp \
../src/disco/OutFile.cpp \
../src/disco/PMDump.cpp \
../src/disco/SET.cpp \
../src/disco/SETpp.cpp \
../src/disco/STR.cpp \
../src/disco/disco_all.cpp \
../src/disco/wb_obj.cpp 

OBJS += \
./src/disco/ASN1.o \
./src/disco/ASN1File.o \
./src/disco/ASN1pp.o \
./src/disco/AnyType.o \
./src/disco/CDPpp.o \
./src/disco/CHAINANY.o \
./src/disco/ChString.o \
./src/disco/ChainA.o \
./src/disco/Convert.o \
./src/disco/DIO.o \
./src/disco/DString.o \
./src/disco/DateTime.o \
./src/disco/Halt.o \
./src/disco/InFile.o \
./src/disco/MachDep.o \
./src/disco/NESTANY.o \
./src/disco/NestA.o \
./src/disco/OutFile.o \
./src/disco/PMDump.o \
./src/disco/SET.o \
./src/disco/SETpp.o \
./src/disco/STR.o \
./src/disco/disco_all.o \
./src/disco/wb_obj.o 

CPP_DEPS += \
./src/disco/ASN1.d \
./src/disco/ASN1File.d \
./src/disco/ASN1pp.d \
./src/disco/AnyType.d \
./src/disco/CDPpp.d \
./src/disco/CHAINANY.d \
./src/disco/ChString.d \
./src/disco/ChainA.d \
./src/disco/Convert.d \
./src/disco/DIO.d \
./src/disco/DString.d \
./src/disco/DateTime.d \
./src/disco/Halt.d \
./src/disco/InFile.d \
./src/disco/MachDep.d \
./src/disco/NESTANY.d \
./src/disco/NestA.d \
./src/disco/OutFile.d \
./src/disco/PMDump.d \
./src/disco/SET.d \
./src/disco/SETpp.d \
./src/disco/STR.d \
./src/disco/disco_all.d \
./src/disco/wb_obj.d 


# Each subdirectory must supply rules for building sources it contributes
src/disco/%.o: ../src/disco/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


