################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../chromatic_strings.cpp \
../chromatic_strings_unit.cpp 

OBJS += \
./chromatic_strings.o \
./chromatic_strings_unit.o 

CPP_DEPS += \
./chromatic_strings.d \
./chromatic_strings_unit.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/media/wiljoh/55FE-3B01/chromatic_universe/trunk/c++/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


