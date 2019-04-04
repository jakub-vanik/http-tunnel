################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../link/client_link.cpp \
../link/link.cpp \
../link/queue.cpp \
../link/server_link.cpp 

OBJS += \
./link/client_link.o \
./link/link.o \
./link/queue.o \
./link/server_link.o 

CPP_DEPS += \
./link/client_link.d \
./link/link.d \
./link/queue.d \
./link/server_link.d 


# Each subdirectory must supply rules for building sources it contributes
link/%.o: ../link/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


