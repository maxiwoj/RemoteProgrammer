##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [2.30.0] date: [Mon Oct 15 23:30:01 CEST 2018] 
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
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Src/usbh_diskio.c \
Src/fatfs.c \
Src/freertos.c \
Src/usb_host.c \
Src/usbh_conf.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_eth.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_hcd.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Src/system_stm32f4xx.c \
Middlewares/Third_Party/FatFs/src/option/syscall.c \
Middlewares/Third_Party/FatFs/src/diskio.c \
Middlewares/Third_Party/FatFs/src/ff.c \
Middlewares/Third_Party/FatFs/src/ff_gen_drv.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
Middlewares/Third_Party/FreeRTOS/Source/list.c \
Middlewares/Third_Party/FreeRTOS/Source/timers.c \
Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
Middlewares/Third_Party/FreeRTOS/Source/queue.c \
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c \
Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_core.c \
Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ctlreq.c \
Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_ioreq.c \
Middlewares/ST/STM32_USB_Host_Library/Core/Src/usbh_pipes.c \
Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c \
Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.c \
Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.c \
Src/syscalls.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/auth.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ccp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/chap_ms.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/chap-md5.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/chap-new.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/demand.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/eap.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/eui64.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/fsm.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ipcp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ipv6cp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/lcp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/magic.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/mppe.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/multilink.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ppp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppapi.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppcrypt.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppoe.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppol2tp.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/pppos.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/upap.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/utils.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/vj.c \
Middlewares/Third_Party/LwIP/src/netif/ethernet.c \
Middlewares/Third_Party/LwIP/src/netif/slipif.c \
Middlewares/Third_Party/LwIP/src/netif/lowpan6.c \
Middlewares/Third_Party/LwIP/src/netif/ppp/ecp.c \
Middlewares/Third_Party/LwIP/src/api/netifapi.c \
Middlewares/Third_Party/LwIP/src/api/tcpip.c \
Middlewares/Third_Party/LwIP/src/api/netbuf.c \
Middlewares/Third_Party/LwIP/src/api/api_msg.c \
Middlewares/Third_Party/LwIP/src/api/api_lib.c \
Middlewares/Third_Party/LwIP/src/api/netdb.c \
Middlewares/Third_Party/LwIP/src/api/sockets.c \
Middlewares/Third_Party/LwIP/src/api/err.c \
Middlewares/Third_Party/LwIP/src/core/def.c \
Middlewares/Third_Party/LwIP/src/core/timeouts.c \
Middlewares/Third_Party/LwIP/src/core/netif.c \
Middlewares/Third_Party/LwIP/src/core/sys.c \
Middlewares/Third_Party/LwIP/src/core/raw.c \
Middlewares/Third_Party/LwIP/src/core/pbuf.c \
Middlewares/Third_Party/LwIP/src/core/memp.c \
Middlewares/Third_Party/LwIP/src/core/tcp.c \
Middlewares/Third_Party/LwIP/src/core/tcp_in.c \
Middlewares/Third_Party/LwIP/src/core/init.c \
Middlewares/Third_Party/LwIP/src/core/mem.c \
Middlewares/Third_Party/LwIP/src/core/stats.c \
Middlewares/Third_Party/LwIP/src/core/inet_chksum.c \
Middlewares/Third_Party/LwIP/src/core/tcp_out.c \
Middlewares/Third_Party/LwIP/src/core/udp.c \
Middlewares/Third_Party/LwIP/src/core/dns.c \
Middlewares/Third_Party/LwIP/src/core/ip.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c \
Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.c \
Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.c \
Middlewares/Third_Party/LwIP/system/OS/sys_arch.c \
Middlewares/Third_Party/LwIP/src/apps/mqtt/mqtt.c \
Src/stm32f4xx_hal_timebase_TIM.c \
Src/main.c \
Src/lwip.c \
Src/ethernetif.c \
Src/stm32f4xx_it.c \
Src/stm32f4xx_hal_msp.c \
Src/communication/dbgu.c \
Src/communication/term_io.c \
Src/communication/binary_download.c \
Src/communication/wakaama_client/wakaama.c \
Src/communication/yuarel.c \
Src/jtag/jtag_low_level.c \
Src/jtag/jtag_scan.c \
Src/adiv5/adiv5_jtag.c \
Src/adiv5/adiv5.c \
Src/cortexm/cortexm.c \
Src/cortexm/stm32/stm32f4.c \
Src/cortexm/stm32/stm32l4.c \
Src/target.c

# ASM sources
ASM_SOURCES =  \
startup_stm32f429xx.s

#######################################
# Wakaama
######################################

WAKAAMA_SOURCES = \
Middlewares/Third_Party/wakaama/liblwm2m.c \
Middlewares/Third_Party/wakaama/wakaama_utils.c \
Middlewares/Third_Party/wakaama/uri.c \
Middlewares/Third_Party/wakaama/objects.c \
Middlewares/Third_Party/wakaama/tlv.c \
Middlewares/Third_Party/wakaama/data.c \
Middlewares/Third_Party/wakaama/wakaama_list.c \
Middlewares/Third_Party/wakaama/packet.c \
Middlewares/Third_Party/wakaama/transaction.c \
Middlewares/Third_Party/wakaama/registration.c \
Middlewares/Third_Party/wakaama/bootstrap.c \
Middlewares/Third_Party/wakaama/management.c \
Middlewares/Third_Party/wakaama/observe.c \
Middlewares/Third_Party/wakaama/json.c \
Middlewares/Third_Party/wakaama/discover.c \
Middlewares/Third_Party/wakaama/block1.c \
Middlewares/Third_Party/wakaama/er-coap-13/er-coap-13.c

WAKAAMA_CUSTOM = \
Src/communication/wakaama_client/platform/platform.c \
Src/communication/wakaama_client/connection.c \
Src/communication/wakaama_client/objects/object_device.c \
Src/communication/wakaama_client/objects/object_security.c \
Src/communication/wakaama_client/objects/object_server.c \
Src/communication/wakaama_client/objects/test_object.c \
Src/communication/wakaama_client/objects/object_target.c


WAKAAMA_INC = \
-IMiddlewares/Third_Party/wakaama \
-IMiddlewares/Third_Party/wakaama/er-coap-13 \


WAKAAMA_SYMBOL = -DLWM2M_LITTLE_ENDIAN
WAKAAMA_SYMBOL += -DLWM2M_CLIENT_MODE
#WAKAAMA_SYMBOL += -DLWM2M_MEMORY_TRACE
#WAKAAMA_SYMBOL += -DCOAPLOG # download logs
#WAKAAMA_SYMBOL += -DLWM2M_WITH_LOGS  # LOGS

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
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
AS_INCLUDES =  \
-I/Inc

# C includes
C_INCLUDES =  \
-IInc \
-IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
-IMiddlewares/ST/STM32_USB_Host_Library/Core/Inc \
-IMiddlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc \
-IMiddlewares/Third_Party/FatFs/src \
-IMiddlewares/Third_Party/FreeRTOS/Source/include \
-IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
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
-IDrivers/CMSIS/Include \
-IInc/communication \
-IInc/communication/wakaama_client \
-IInc/communication/wakaama_client/objects \
-Iconfig

C_INCLUDES += $(WAKAAMA_INC)

C_SOURCES += $(WAKAAMA_SOURCES) $(WAKAAMA_CUSTOM)

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) $(WAKAAMA_SYMBOL) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


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

program: all
	openocd -f board/st_nucleo_f4.cfg -c "program build/RemoteProgrammer.elf verify reset exit"

st-flash: all
	st-flash write build/RemoteProgrammer.bin 0x8000000

debug:
	arm-none-eabi-gdb ./build/RemoteProgrammer.elf

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
