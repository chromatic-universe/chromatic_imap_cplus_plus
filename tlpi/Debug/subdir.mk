################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tlpi.cpp \
../tlpi_authentication_assist.cpp \
../tlpi_credentials.cpp 

OBJS += \
./tlpi.o \
./tlpi_authentication_assist.o \
./tlpi_credentials.o 

CPP_DEPS += \
./tlpi.d \
./tlpi_authentication_assist.d \
./tlpi_credentials.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/media/wiljoh/55FE-3B01/chromatic_universe/trunk/c++/include -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


