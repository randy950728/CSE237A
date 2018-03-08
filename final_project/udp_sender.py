import cv2
import sys
import socket
import numpy as np
global sock

#def socket_init():
# ---------------------------------------------------------------------#
def socket_init(UDP_IP, UDP_PORT):
	global sock
	#sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
	sock.connect((UDP_IP, UDP_PORT))
	return


# ---------------------------------------------------------------------#
def transmit_data(message, pack_size, sock, UDP_IP, UDP_PORT):

	off_set = 0
	total_size = sys.getsizeof(message)
	num_packet = total_size/pack_size
	print(message[:10])
	for i in range(num_packet):
		#bytes_sent = sock.sendto(message[off_set:(off_set+pack_size)], (UDP_IP, UDP_PORT))
		bytes_sent = sock.send(message[off_set:(off_set+pack_size)])
		print("=====sending : " + str(bytes_sent))
		off_set   +=bytes_sent
		total_size-=bytes_sent

	if(total_size!=0):
		#sock.sendto(message[off_set:], (UDP_IP, UDP_PORT))
		sent = sock.send(message[off_set:])
	return


# ---------------------------------------------------------------------#
def socket_send(UDP_IP, UDP_PORT, pack_size, MESSAGE, unknown, full=None):
	assert isinstance(MESSAGE, dict) or MESSAGE==None
	global sock
	total_msg = str()

	if(unknown==True):
		# Calculate Image size
		image_string = cv2.imencode('.jpg', full)[1].tostring()
		image_len = sys.getsizeof(image_string)

		# Append message into string variable
		total_msg = "CSE237A"
		total_msg += "00"
		total_msg += '{0:07d}'.format(image_len)
		total_msg += image_string
		print("unknown: " + str(sys.getsizeof(total_msg)))

		# Start transfering message
		transmit_data(total_msg, pack_size, sock, UDP_IP, UDP_PORT)
		total_msg = str()

	else:
		for key in MESSAGE:
			if(MESSAGE[key][2]==0):
				# Mark image as sent
				MESSAGE[key][2]=1
				#cv2.imshow("send face", MESSAGE[key][0])
				print("message: " + str(sys.getsizeof(MESSAGE[key][0])))

				# Calculate Image size
				image_string = cv2.imencode('.jpg', MESSAGE[key][0])[1].tostring()
				image_len = sys.getsizeof(image_string)

				# Append message into string variable
				total_msg = "CSE237A"
				total_msg += "01"
				total_msg += '{0:07d}'.format(image_len)
				total_msg += image_string
				print("known: " + str(sys.getsizeof(total_msg)))

				# Start transfering message
				transmit_data(total_msg, pack_size, sock, UDP_IP, UDP_PORT)
				total_msg = str()
	return
