################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../main.cpp \
../proxy.cpp \
../proxy_connection.cpp \
../relay.cpp \
../relay_connection.cpp \
../service.cpp 

OBJS += \
./main.o \
./proxy.o \
./proxy_connection.o \
./relay.o \
./relay_connection.o \
./service.o 

CPP_DEPS += \
./main.d \
./proxy.d \
./proxy_connection.d \
./relay.d \
./relay_connection.d \
./service.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


