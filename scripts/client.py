#!/usr/bin/env python3
"""
S3K Proof-of-Concept client application
"""

from argparse import ArgumentParser
from queue import Queue
from threading import Thread
import socket
import time
import sys
import shlex
import string
import shutil


class BadCommand(Exception):
    """
    Thrown when bad command was inputted
    """


def ppp_encode(data):
    """
    Encode data as a PPP frame.
    """
    data = bytearray(data)
    data = data.replace(b"\\", b"\\|")
    data = data.replace(b"\n", b"\\*")
    return data


def ppp_decode(data):
    """
    Decode a PPP frame.
    """
    data = bytearray(data)
    res = []
    escape = False
    for i in data:
        if i == ord('\\'):
            escape = True
        else:
            res.append(i if not escape
                       else i ^ 0x20)
            escape = False
    return res


def make_ppp_frames(tag, data):
    """
    Divide data into multiple PPP frames that includes a header.
    Each frame becomes,
    <tag>:<offset>:<payload>
    where <tag> identifies type of package,
          <offset> is offset info _data_,
          <payload> is part of _data_.
    """
    # Calculate the payload size
    header_len = len(tag + b":0000:")
    payload_len = FRAME_LEN - header_len
    payload_len -= payload_len % 8

    # Create frames with our header.
    # Header is '<receiver>:<offset>:'
    frames = []
    for i in range(0, len(data), payload_len):
        offset = bytes(format(i, '04x'), 'ASCII')
        header = tag + b":" + offset + b":"
        payload = data[i:i+payload_len]
        frame = ppp_encode(header + payload)
        frames.append(frame)

    # Add frame markers (b'\n') between each frame
    return b"\n" + b"\n".join(frames) + b"\n"


def parse_command(cmd):
    """
    Parse command from input and get receiver + data to send.
    """
    try:
        cmd_args = shlex.split(cmd)
    except ValueError as excpt:
        raise BadCommand(f"Command Error: {excpt}.") from excpt

    if len(cmd_args) != 3:
        raise BadCommand("Command Error: Expected <cmd> <tag> <argument>.")

    cmd = cmd_args[0]
    tag = cmd_args[1].encode('ASCII')

    if len(tag) > 8:
        # Tag is at most 8 characters
        raise BadCommand(f"Command Error: Tag '{tag}' > 8.")

    if cmd == 'send':
        data = cmd_args[2].encode('ASCII')
        return (tag, data)

    if cmd == 'send-file':
        filename = cmd_args[2]
        try:
            with open(filename, 'rb') as file:
                data = file.read()
                return (tag, data)
        except OSError as excpt:
            raise BadCommand(f"Command Error: {excpt.strerror}.") from excpt

    raise BadCommand("Command Error: Unknown command.")


def receiver_worker(sock):
    """
    Receive frames from the kernel apps and prints them.
    """
    line = b""
    while True:
        data = sock.recv(256)
        data = data.split(b"\n")
        if len(data) == 1:
            line += data[0]
        else:
            data[0] = line + data[0]
            line = data[-1]

        for frame in data[0:-1]:
            if not frame:
                continue
            # Decode the frame
            payload = ppp_decode(frame)
            # Replace non-printable bytes with period
            text = [i if i in bytes(string.printable[:-5], 'ASCII')
                    else ord('.') for i in payload]
            term_size = shutil.get_terminal_size((80, 20))
            text = ">" + bytes(text).decode('ASCII')
            print(text[:term_size.columns])


def sender_worker(sock, data_queue):
    """
    Makes frames from (receiver, data) and sends
    to the kernel apps.
    """
    while True:
        receiver, data = data_queue.get(block=True)
        frames = make_ppp_frames(receiver, data)
        sock.sendall(frames)
        print(f"Sent {len(data)} bytes.")


def main(ip_addr, port):
    """
    Main function of this module
    """

    data_queue = Queue()

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip_addr, port))

    receiver = Thread(target=receiver_worker,
                      args=(sock,),
                      daemon=True)
    sender = Thread(target=sender_worker,
                    args=(sock, data_queue,),
                    daemon=True)

    receiver.start()
    sender.start()

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            tag, data = parse_command(line)
        except BadCommand as excpt:
            print(excpt)
            continue
        data_queue.put((tag, data))

    socket.close()


if __name__ == "__main__":
    parser = ArgumentParser(
            prog="s3k-poc-client",
            description="Client for communicating with PoC applications")
    parser.add_argument('ip_addr', type=str)
    parser.add_argument('port', type=int)
    parser.add_argument('-f', '--frame-len', type=int, default=256)
    args = parser.parse_args()
    FRAME_LEN = args.frame_len

    main(args.ip_addr, args.port)
