#!/usr/bin/python
import socket
host = '127.0.0.1'
port = 18911
BUFSIZE = 1024
tcpClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcpClient.connect((host, port))
while True:
	data = raw_input('>enter:')
	if not data:
		break
	tcpClient.send(data)
	data = tcpClient.recv(BUFSIZE)
	if not data:
		break
tcpClient.close()
