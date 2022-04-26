import sys
import socket
from common import *
import enum
import time

# enum class for the different arguments
class const(enum.IntEnum):
    name = 0
    host = 1
    port = 2
    expected_argc = 3
    light_packet_size = 16
    stons = 1000000000

def main():
    # checks the number of arguments
    if (len(sys.argv) != const.expected_argc):
        print("Usage: python3 ", sys.argv[const.name], " <host> <port>")
        return -1
    # creates a new client socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # connects to the server
    client_socket.connect((sys.argv[const.host], int(sys.argv[const.port])))
    # dummy light packet
    seq = 0
    status = 0
    # loops forever
    while True:
        # sends the packet
        ns = time.monotonic_ns()
        seq = seq + 1
        status = not status
        packet = light_packet(timespec(ns // const.stons, ns % const.stons), seq, status)
        client_socket.send(packet.to_raw())
        time.sleep(1)

if __name__ == '__main__':
    main()