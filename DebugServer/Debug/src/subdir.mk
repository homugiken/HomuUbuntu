################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../src/DebugClient.c \
../src/DebugServer.c \
../src/dbgmsg.c \
../src/dbgstd.c \
../src/general.c \
../src/main.c

OBJS += \
./src/DebugClient.o \
./src/DebugServer.o \
./src/dbgmsg.o \
./src/dbgstd.o \
./src/general.o \
./src/main.o

C_DEPS += \
./src/DebugClient.d \
./src/DebugServer.d \
./src/dbgmsg.d \
./src/dbgstd.d \
./src/general.d \
./src/main.d

GIT_COMMIT=$(shell git describe --always --dirty)

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -Wextra -DGITCOMMIT=\"$(GIT_COMMIT)\" -Wno-comment -Wno-unused-label -Wno-unused-function -Wno-unused-parameter -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


