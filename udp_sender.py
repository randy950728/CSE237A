import cv2
import sys
import socket

global sock

def socket_init():
	global sock
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	return

def socket_send(UDP_IP, UDP_PORT, pack_size, MESSAGE):
	assert isinstance(MESSAGE, dict)

	global sock
	total_msg = str()
	for key in MESSAGE:
		if(MESSAGE[key][2]==0):
			total_msg += cv2.imencode('.png', MESSAGE[key][0])[1].tostring()
			MESSAGE[key][2]=1
			off_set = 0
			total_size = sys.getsizeof(total_msg)
			num_packet = total_size/pack_size
			for i in range(num_packet):
				bytes_sent = sock.sendto(total_msg[off_set:(off_set+pack_size)], (UDP_IP, UDP_PORT))
				off_set +=bytes_sent
				total_size-=bytes_sent
			if(total_size!=0):
				sock.sendto(total_msg[off_set:], (UDP_IP, UDP_PORT))
			total_msg=str()
	return
