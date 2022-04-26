# timespec class
from sys import byteorder


class timespec():
    def __init__(self, sec, nsec):
        self.tv_sec = sec
        self.tv_nsec = nsec
    def from_raw(packet):
        return timespec(int.from_bytes(packet[0:4], byteorder='little'), int.from_bytes(packet[4:8], byteorder='little'))


# light_packet class
class light_packet():
    def __init__(self, timestamp, sequence, data):
        self.timestamp = timestamp
        self.sequence = sequence
        self.data = data
    def from_raw(packet):
        return light_packet(timespec.from_raw(packet[0:8]), int.from_bytes(packet[8:12], byteorder='little'), int.from_bytes(packet[12:16], byteorder='little'))
    def to_raw(self):
        return self.timestamp.tv_sec.to_bytes(4, byteorder='little') + self.timestamp.tv_nsec.to_bytes(4, byteorder='little') + self.sequence.to_bytes(4, byteorder='little') + self.data.to_bytes(4, byteorder='little')