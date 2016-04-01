USER_OBJS :=
LIBS := -ligraph
RM := rm -f
# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./src/export_nodes.c \
./src/main.c 

OBJS += \
./src/export_nodes.o \
./src/main.o 

C_DEPS += \
./src/export_nodes.d \
./src/main.d 

all: graphml2metis

# Tool invocations
graphml2metis: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "graphml2metis" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) graphml2metis
	-@echo ' '

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/igraph -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


