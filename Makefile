##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [2.27.0] date: [Sat May 19 16:05:30 CEST 2018] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = RemoteProgrammer


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# source path
SOURCES_DIR =  \
Drivers/STM32F4xx_HAL_Driver \
Application \
Application/User \
Middlewares \
Drivers/CMSIS \
Middlewares/LwIP \
Drivers

# firmware library path
PERIFLIB_PATH = 

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Middlewares/Third_Party/LwIP/src/netif/ppp/ipcp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/chap_ms.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c \
Middlewares/Third_Party/LwIP/src/core/inet_chksum.c \
Middlewares/Third_Party/LwIP/src/core/stats.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.c \
Middlewares/Third_Party/LwIP/src/api/netdb.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/chap-md5.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/vj.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/lcp.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.c \
/Src/system_stm32f4xx.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppos.c \
Middlewares/Third_Party/LwIP/src/core/timeouts.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c \
Middlewares/Third_Party/LwIP/src/api/tcpip.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/eap.c \
Middlewares/Third_Party/LwIP/src/core/sys.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.c \
Middlewares/Third_Party/LwIP/src/api/api_msg.c \
Middlewares/Third_Party/LwIP/src/netif/lowpan6.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/magic.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/upap.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/eui64.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ecp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/multilink.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppcrypt.c \
Middlewares/Third_Party/LwIP/src/core/pbuf.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ppp.c \
Middlewares/Third_Party/LwIP/src/core/mem.c \
Middlewares/Third_Party/LwIP/src/core/init.c \
Middlewares/Third_Party/LwIP/src/core/udp.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Middlewares/Third_Party/LwIP/src/core/tcp_out.c \
Src/stm32f4xx_it.c \
Src/stm32f4xx_hal_msp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/mppe.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.c \
Middlewares/Third_Party/LwIP/src/apps/mqtt/mqtt.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/chap-new.c \
Middlewares/Third_Party/LwIP/src/api/api_lib.c \
Middlewares/Third_Party/LwIP/src/api/sockets.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/fsm.c \
Middlewares/Third_Party/LwIP/src/core/raw.c \
Middlewares/Third_Party/LwIP/src/api/netbuf.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppol2tp.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Middlewares/Third_Party/LwIP/src/core/memp.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c \
Middlewares/Third_Party/LwIP/src/api/netifapi.c \
Src/main.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Src/ethernetif.c \
Middlewares/Third_Party/LwIP/src/core/dns.c \
Middlewares/Third_Party/LwIP/src/core/ip.c \
Middlewares/Third_Party/LwIP/src/netif/slipif.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppapi.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Src/lwip.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ccp.c \
Middlewares/Third_Party/LwIP/src/core/def.c \
Middlewares/Third_Party/LwIP/src/core/tcp_in.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/demand.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c \
Middlewares/Third_Party/LwIP/src/core/netif.c \
Middlewares/Third_Party/LwIP/src/netif/ethernet.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppoe.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/utils.c \
Middlewares/Third_Party/LwIP/src/api/err.c \
Middlewares/Third_Party/LwIP/src/core/tcp.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ipv6cp.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/auth.c

# ASM sources
ASM_SOURCES =  \
startup_stm32f429xx.s


######################################
# firmware library
######################################
PERIFLIB_SOURCES = 


#######################################
# binaries
#######################################
BINPATH = 
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
AR = $(PREFIX)ar
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F429xx


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-IInc \
-IMiddlewares/Third_Party/LwIP/src/include \
-IMiddlewares/Third_Party/LwIP/system \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/Third_Party/LwIP/src/include/netif/ppp \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IMiddlewares/Third_Party/LwIP/src/include/lwip \
-IMiddlewares/Third_Party/LwIP/src/include/lwip/apps \
-IMiddlewares/Third_Party/LwIP/src/include/lwip/priv \
-IMiddlewares/Third_Party/LwIP/src/include/lwip/prot \
-IMiddlewares/Third_Party/LwIP/src/include/netif \
-IMiddlewares/Third_Party/LwIP/src/include/posix \
-IMiddlewares/Third_Party/LwIP/src/include/posix/sys \
-IMiddlewares/Third_Party/LwIP/system/arch \
-IDrivers/CMSIS/Include


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F429ZITx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
