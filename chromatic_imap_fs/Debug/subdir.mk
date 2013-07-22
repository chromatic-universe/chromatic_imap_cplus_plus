################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../chromatic_fs.cpp 

OBJS += \
./chromatic_fs.o 

CPP_DEPS += \
./chromatic_fs.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=27 -I/usr/local/include/fusekit -I./ -I/dev_tools/SQLAPI/include -I/dev_tools/ACE_wrappers -I/dev_tools/chilkat-9.4.0-x86-linux-gcc/include -I/media/wiljoh/55FE-3B01/chromatic_universe_gcc_11/trunk/include -I/media/wiljoh/55FE-3B01/chromatic_universe_gcc_11/trunk/chromatic_imap -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


