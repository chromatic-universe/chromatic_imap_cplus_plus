################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../chromatic_acceptor.cpp \
../chromatic_imap.cpp \
../chromatic_imap_handler.cpp \
../chromatic_imap_message_manip.cpp \
../chromatic_imap_mgr.cpp \
../chromatic_imap_search.cpp \
../chromatic_thread_pool.cpp 

OBJS += \
./chromatic_acceptor.o \
./chromatic_imap.o \
./chromatic_imap_handler.o \
./chromatic_imap_message_manip.o \
./chromatic_imap_mgr.o \
./chromatic_imap_search.o \
./chromatic_thread_pool.o 

CPP_DEPS += \
./chromatic_acceptor.d \
./chromatic_imap.d \
./chromatic_imap_handler.d \
./chromatic_imap_message_manip.d \
./chromatic_imap_mgr.d \
./chromatic_imap_search.d \
./chromatic_thread_pool.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DACE_NTRACE=0 -I/media/wiljoh/55FE-3B01/chromatic_universe_gcc_11/trunk/include -I/dev_tools/chilkat-9.4.0-x86-linux-gcc/include -I/dev_tools/SQLAPI/include -I/dev_tools/ACE_wrappers -O0 -g3 -Wall -c -fmessage-length=0  -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


