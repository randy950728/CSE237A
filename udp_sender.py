import cv2
import sys
import socket

global sock

def socket_init():
	global sock
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	return

def socket_send(UDP_IP, UDP_PORT, pack_size, MESSAGE, unknown, full=None):
	assert isinstance(MESSAGE, dict) or isinstance(MESSAGE, none)

	global sock
	num_img = 0
	total_msg = str()
	if(unknown==True):
		total_msg += "00"
		total_msg += cv2.imencode('.png', full)[1].tostring()
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

	else:
		total_msg += "01"
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
				total_msg="01"
	return
