#!/usr/bin/python3

import socket
import errno
import sys
import binascii
import time

import sequencer
from axis_fifo import AXIS_FIFO
from devices import fifo_devices
from devices import gpio_devices
from axi_gpio import AXI_GPIO
from reset_all import reset
from soft_trigger import trigger
from arm_trigger import armtrigger
from disable_trigger import disabletrigger
import dds_lock_pll

from dac81416 import DAC81416

class zynq_tcp_server:
	def __init__(self):
		self.seq = sequencer.sequencer()
		self.dioByteLen = 28
		self.dacByteLen = 44
		self.ddsByteLen = 52

		self.chimeraInterface()

	#Function that reads commands from Chimera and passes them to the axis-fifo interface of the Zynq FPGA
	def chimeraInterface(self):
		# Create a TCP/IP socket
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

		# Bind the socket to the port
		server_address = ("10.10.0.2", 8080)
		print('server starting up on %s port %s' % server_address)
		sock.bind(server_address)

		# Listen for incoming connections
		sock.listen(1)

		while True:
			# Wait for a connection
			print('waiting for a connection')
			connection, client_address = sock.accept()

			try:
				print('connection from', client_address)

				# Receive the data in small chunks
				while True:
					data = connection.recv(64).decode('utf-8')
					# data = data.split('\0')[0]
					print('received "%s"' % data)
					# print(len(data)))
					if data:
						self.writeDevice(connection, data)
						# connection.sendall(data)
					else:
						# print('no more data from', client_address)
						break
			except socket.error as error:
				print(error)
				break

		connection.close()

	def writeDevice(self, conn, data):
		data = data.strip('\0')
		data_split = data.split('_')
		dev = data_split[0]
		print('dev = ', dev)
		if (dev == 'DIOseq'):
			self.writeDIOseq(conn, data_split)
			self.seq.mod_enable()
		elif (dev == 'DACseq'):
			self.writeDACseq(conn, data_split)
			self.seq.mod_enable()
		elif (dev == 'DDSseq'):
			self.writeDDSseq(conn, data_split)
			self.seq.mod_enable()
		elif (dev == 'initExp'):
			self.seq.initExp()
		elif (dev == 'disableSeq'):
			self.seq.reset_disable_mod()
		elif (dev == 'disableMod'):
			self.seq.mod_disable()
		elif (dev == 'DAC'):
			self.seq.set_DAC(int(data_split[1]), float(data_split[2]))
		elif (dev == 'DDS'):
			self.seq.set_DDS(int(data_split[1]), float(data_split[2]))
		elif (dev == 'lockPLL'):
			self.seq.lock_PLL()
		elif (dev == 'trigger'):
			self.seq.soft_trigger()
		elif (dev == 'armtrigger'):
			self.seq.arm_trigger()
		elif (dev == 'disabletrigger'):
			self.seq.disable_trigger()
		else:
			print('no device selected')



	def writeDIOseq(self, conn, data_split):
		num_snapshots = int(data_split[1])
		print('num_bytes = ', self.dioByteLen*num_snapshots)
		byte_buf = self.socket_read(conn, self.dioByteLen*num_snapshots) #each byte buffer snapshot consists of 3 sets of 4 bytes
		# print(hex(ord(byte_buf[0])))
		for ii in range(num_snapshots):
			print('\n', 'snapshot', ii)
			print(byte_buf[ii*self.dioByteLen: ii*self.dioByteLen + self.dioByteLen])
		self.seq.dio_seq_write_points(self.dioByteLen, byte_buf, num_snapshots)

	def writeDACseq(self, conn, data_split):
		num_snapshots = int(data_split[1])
		print('num_snapshots = ', num_snapshots)
		byte_buf = self.socket_read(conn, self.dacByteLen*num_snapshots)
		for ii in range(num_snapshots):
			print('\n', 'snapshot', ii)
			print(byte_buf[ii*self.dacByteLen: ii*self.dacByteLen + self.dacByteLen])
		self.seq.dac_seq_write_points(self.dacByteLen, byte_buf, num_snapshots)

	def writeDDSseq(self, conn, data_split):
		num_snapshots = int(data_split[1])
		print('num_snapshots = ', num_snapshots)
		byte_buf = self.socket_read(conn, self.ddsByteLen*num_snapshots)
		for ii in range(num_snapshots):
			print('\n', 'snapshot', ii)
			print(byte_buf[ii*self.ddsByteLen: ii*self.ddsByteLen + self.ddsByteLen])
		print("self.ddsByteLen = ", self.ddsByteLen)
		print("byte_buf = ", byte_buf)
		self.seq.dds_seq_write_points(self.ddsByteLen, byte_buf, num_snapshots)

	def socket_read(self, conn, expected):
		"""Read expected number of bytes from sock
		Will repeatedly call recv until all expected data is received
		"""
		buffer = b''
		while len(buffer) < expected:
			buffer += conn.recv(expected - len(buffer))
		return buffer

if __name__ == "__main__":
	server = zynq_tcp_server()
