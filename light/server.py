import sys
import select
import socket
from common import *
import enum
import asyncio
import libhueble
import signal

# enum class for the different arguments
class const(enum.IntEnum):
    name = 0
    port = 1
    mac = 2
    expected_argc = 3
    server_backlog = 1
    light_packet_size = 16

should_continue = True

def sig_handler(signum, frame):
    print("Stopping...")
    should_continue = False

async def toggle_light(light, data):
    try:
        await light.set_power(data != 0)
    except Exception as e:
        print(f'Failed to toggle the light: {e}')

def main():
    # checks the number of arguments
    if (len(sys.argv) != const.expected_argc):
        print("Usage: python3 ", sys.argv[const.name], " <port> <mac>")
        return -1
    # register signal handler
    signal.signal(signal.SIGINT, sig_handler)
    # create a new light
    light = libhueble.Lamp(sys.argv[const.mac])
    loop = asyncio.get_event_loop()
    # connects to the light
    try:
        loop.run_until_complete(light.connect())
    except Exception as e:
        print(f'Failed to connect to the light: {e}')
        return -1
    # creates a new server socket
    connection_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # binds to port specified in command line as the first argument
    connection_socket.bind(('', int(sys.argv[const.port])))
    print("Listening on port ", str(sys.argv[const.port]))
    # starts listening for connections
    connection_socket.listen(const.server_backlog)
    # selects from the connection socket and the client sockets
    select_list = [connection_socket]
    # loops while it should continue
    while should_continue:
        # waits for an event
        events = select.select(select_list, [], [])
        # loops through the events
        for selected_socket in events[0]:
            # if the event is a connection
            if selected_socket == connection_socket:
                # accepts the connection
                client_socket, client_address = selected_socket.accept()
                # prints the client address
                print("Client connected from: " + str(client_address))
                # adds the client socket to the select list
                select_list.append(client_socket)
            # if the event is a client socket
            else:
                # reads the packet from the client socket
                raw_packet = selected_socket.recv(const.light_packet_size)
                if not raw_packet:
                    # removes the client socket from the select list
                    select_list.remove(selected_socket)
                    # closes the client socket
                    selected_socket.close()
                else:
                    # converts the packet to a light packet
                    packet = light_packet.from_raw(raw_packet)
                    print("timestamp: ", packet.timestamp.tv_sec, ".", packet.timestamp.tv_nsec, " sequence: ", packet.sequence, " data: ", packet.data)
                    # powers off the light if data is 0, otherwise powers on the light
                    asyncio.run(toggle_light(light, packet.data))
    # closes the connection socket
    connection_socket.close()
    # disconnects the light
    try:
        loop.run_until_complete(light.disconnect())
    except Exception as e:
        print(f'Failed to disconnect the light: {e}')
        return -1
    return 0

if __name__ == '__main__':
    main()