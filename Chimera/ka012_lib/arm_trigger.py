#!/usr/bin/python

from axi_gpio import AXI_GPIO
from devices import gpio_devices
import struct

def armtrigger():
  gpio = AXI_GPIO(gpio_devices['axi_gpio_2'])

  #enable mod
  gpio.clear_bit(0, channel=1)
  gpio.set_bit(0, channel=1)

  #enable arm trigger mode
  gpio.clear_bit(2, channel=1)
  gpio.set_bit(2, channel=1)

  #clear hardware trigger enable
  gpio.clear_bit(4, channel=1)
  #hardware trigger enable
  gpio.set_bit(4, channel=1)

  #clear arm for trigger
  #always do this first as bit doesn't autoclear
  gpio.clear_bit(3, channel=1)
  #arm trigger
  gpio.set_bit(3, channel=1)

  reg=gpio.read_axi_gpio(channel=1)
  print(reg)

  for i in range(4):
    print(reg[i])

if __name__ == "__main__":
  armtrigger()