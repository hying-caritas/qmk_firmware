SRC += matrix.c c1_main.c c1_usbh.c tusb_os_custom.c
CUSTOM_MATRIX = lite
# Only enables the pointing device module; the driver struct is not
# implemented, mouse reports are injected via mouse_report_hook and
# pointing_device_task() in matrix.c (see TODO there)
POINTING_DEVICE_DRIVER = custom

CRT0_EXTRA_CORES_NUMBER = 1

SRC += ../lib/Pico-PIO-USB/src/pio_usb.c
SRC += ../lib/Pico-PIO-USB/src/pio_usb_host.c
SRC += ../lib/Pico-PIO-USB/src/usb_crc.c
VPATH += keyboards/sekigon/keyboard_quantizer/lib/Pico-PIO-USB/src

SRC += ../lib/tinyusb/src/tusb.c
SRC += ../lib/tinyusb/src/common/tusb_fifo.c
SRC += ../lib/tinyusb/src/host/usbh.c
SRC += ../lib/tinyusb/src/host/hub.c
SRC += ../lib/tinyusb/src/class/hid/hid_host.c
SRC += ../lib/tinyusb/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
VPATH += keyboards/sekigon/keyboard_quantizer/lib/tinyusb/src

SRC += lib/pico-sdk/src/rp2_common/hardware_dma/dma.c
SRC += lib/pico-sdk/src/host/pico_stdlib/stdlib.c
VPATH += lib/pico-sdk/src/rp2_common/hardware_dma/include
VPATH += lib/pico-sdk/src/rp2_common/hardware_uart/include
VPATH += lib/pico-sdk/src/rp2_common/pico_stdio/include
VPATH += lib/pico-sdk/src/common/pico_stdlib/include
VPATH += lib/pico-sdk/src/common/pico_time/include
VPATH += lib/pico-sdk/src/common/pico_sync/include
VPATH += lib/pico-sdk/src/common/pico_util/include

GIT_DESCRIBE := $(shell git describe --tags --long --always --dirty="\\*" 2>/dev/null)
CFLAGS += -DGIT_DESCRIBE=$(GIT_DESCRIBE)
