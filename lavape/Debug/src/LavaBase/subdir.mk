################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/LavaBase/AboutBox.o \
../src/LavaBase/BAdapter.o \
../src/LavaBase/DumpView.o \
../src/LavaBase/InterprBase.o \
../src/LavaBase/LavaAppBase.o \
../src/LavaBase/LavaBaseDoc.o \
../src/LavaBase/LavaBaseStringInit.o \
../src/LavaBase/LavaThread.o \
../src/LavaBase/PEBaseDoc.o \
../src/LavaBase/SylTraversal.o \
../src/LavaBase/SynIDTable.o \
../src/LavaBase/SynIO.o \
../src/LavaBase/Syntax.o \
../src/LavaBase/SyntaxG.o \
../src/LavaBase/moc_AboutBox.o \
../src/LavaBase/moc_DumpView.o 

CPP_SRCS += \
../src/LavaBase/AboutBox.cpp \
../src/LavaBase/BAdapter.cpp \
../src/LavaBase/DumpView.cpp \
../src/LavaBase/InterprBase.cpp \
../src/LavaBase/LavaAppBase.cpp \
../src/LavaBase/LavaBaseDoc.cpp \
../src/LavaBase/LavaBaseStringInit.cpp \
../src/LavaBase/LavaBase_all.cpp \
../src/LavaBase/LavaThread.cpp \
../src/LavaBase/PEBaseDoc.cpp \
../src/LavaBase/SylTraversal.cpp \
../src/LavaBase/SynIDTable.cpp \
../src/LavaBase/SynIO.cpp \
../src/LavaBase/Syntax.cpp \
../src/LavaBase/SyntaxG.cpp \
../src/LavaBase/moc_AboutBox.cpp \
../src/LavaBase/moc_DumpView.cpp 

OBJS += \
./src/LavaBase/AboutBox.o \
./src/LavaBase/BAdapter.o \
./src/LavaBase/DumpView.o \
./src/LavaBase/InterprBase.o \
./src/LavaBase/LavaAppBase.o \
./src/LavaBase/LavaBaseDoc.o \
./src/LavaBase/LavaBaseStringInit.o \
./src/LavaBase/LavaBase_all.o \
./src/LavaBase/LavaThread.o \
./src/LavaBase/PEBaseDoc.o \
./src/LavaBase/SylTraversal.o \
./src/LavaBase/SynIDTable.o \
./src/LavaBase/SynIO.o \
./src/LavaBase/Syntax.o \
./src/LavaBase/SyntaxG.o \
./src/LavaBase/moc_AboutBox.o \
./src/LavaBase/moc_DumpView.o 

CPP_DEPS += \
./src/LavaBase/AboutBox.d \
./src/LavaBase/BAdapter.d \
./src/LavaBase/DumpView.d \
./src/LavaBase/InterprBase.d \
./src/LavaBase/LavaAppBase.d \
./src/LavaBase/LavaBaseDoc.d \
./src/LavaBase/LavaBaseStringInit.d \
./src/LavaBase/LavaBase_all.d \
./src/LavaBase/LavaThread.d \
./src/LavaBase/PEBaseDoc.d \
./src/LavaBase/SylTraversal.d \
./src/LavaBase/SynIDTable.d \
./src/LavaBase/SynIO.d \
./src/LavaBase/Syntax.d \
./src/LavaBase/SyntaxG.d \
./src/LavaBase/moc_AboutBox.d \
./src/LavaBase/moc_DumpView.d 


# Each subdirectory must supply rules for building sources it contributes
src/LavaBase/%.o: ../src/LavaBase/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


