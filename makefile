# Add inputs and outputs from these tool invocations to the build variables 
USER_OBJS :=
LIBS := -ligraph -lglib-2.0
RM := rm -f
# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./src/graph_construction.c \
./src/main.c 

OBJS += \
./src/graph_construction.o \
./src/main.o 

C_DEPS += \
./src/graph_construction.d \
./src/main.d 


all: partitions2graph

call: clean partitions2graph

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) graph
	-@echo ' '

# Each subdirectory must supply rules for building sources it contributes
%.o: ./%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/glib-2.0/ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ -I/usr/lib/glib-2.0/include -I/usr/include/igraph -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# Tool invocations
partitions2graph: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "partitions2graph" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '


