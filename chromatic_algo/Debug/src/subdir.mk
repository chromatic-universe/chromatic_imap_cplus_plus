################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/chromatic_algo_impl.cpp \
../src/chromatic_algo_link.cpp 

OBJS += \
./src/chromatic_algo_impl.o \
./src/chromatic_algo_link.o 

CPP_DEPS += \
./src/chromatic_algo_impl.d \
./src/chromatic_algo_link.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


