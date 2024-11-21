#!/usr/bin/python

from axi_gpio import AXI_GPIO
from devices import gpio_devices
import struct

def disabletrigger():
  gpio = AXI_GPIO(gpio_devices['axi_gpio_2'])

  #enable mod
  gpio.set_bit(0, channel=1)
  #clear harware trigger enable
  gpio.clear_bit(4, channel=1)

if __name__ == "__main__":
  disabletrigger()