#!/usr/bin/env python3
"""
S3K Proof-of-Concept client application
"""

from argparse import ArgumentParser
from queue import Queue
import threading
import socket
import time
import sys
import shlex
import string
import shutil

def ppp_encode(data):
    data = bytearray(data)
    data = data.replace(b"\\", b"\\|")
    data = data.replace(b"\n", b"\\*")
    return data

def ppp_decode(data):
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

def make_ppp_frames(receiver, data):
    n = FRAME_SIZE - len(receiver) - 14
    n -= n % 8
    data = bytearray(data)
    frames = [ppp_encode(receiver + b":" + bytes(format(i, "04x"), 'ascii') + b":" + data[i:i+n])
              for i in range(0, len(data), n)]
    return b"\n" + b"\n".join(frames) + b"\n"

class Receiver(threading.Thread):
    def __init__(self, sock):
        threading.Thread.__init__(self)
        self.sock = sock
        self.daemon = True
        self.name = "receiver"

    def run(self):
        line = b""
        while True:
            data = self.sock.recv(256)
            data = data.split(b"\n")
            if len(data) == 1:
                line += data[0]
            else:
                data[0] = line + data[0]
                line = data[-1]
            for frame in data[0:-1]:
                if not frame:
                    continue
                # Replace non-printable bytes with period
                frame = ppp_decode(frame)
                text = [i if i in bytes(string.printable[:-5], 'ascii')
                        else ord('.') for i in frame]
                term_size = shutil.get_terminal_size((80, 20))
                text = ">" + bytes(text).decode('ascii')
                print(text[:term_size.columns])

class Sender(threading.Thread):
    def __init__(self, sock, cmd_queue):
        threading.Thread.__init__(self)
        self.sock = sock
        self.daemon = True
        self.name = "sender"
        self.cmd_queue = cmd_queue

    def parse_command(self, cmd):
        args = shlex.split(cmd)
        if args[0] == 'send' and len(args) == 3 and len(args[1]) <= 8:
            receiver = args[1].encode('ascii')
            data = args[2].encode('ascii')
            return (receiver, data)
        elif args[0] == 'send-file' and len(args) == 3 and len(args[1]) <= 8:
            receiver = args[1].encode('ascii')
            filename = args[2]
            with open(filename, 'rb') as file:
                data = file.read()
            return (receiver, data)
        else:
            return None

    def run(self):
        while True:
            try:
                cmd = self.cmd_queue.get(block=True)
                res = self.parse_command(cmd)
                if not res:
                    continue
                receiver, data = res
                if len(data) > 0x10000:
                    print(f"Too much data, {len(data)} bytes.")
                elif data:
                    frames = make_ppp_frames(receiver, data)
                    self.sock.sendall(frames)
                    print(f"Sent {len(data)} bytes.")
                else:
                    print("Invalid command")
            except Exception as e:
                print(e)


def main(ip_addr, port):
    """
    Main function of this module
    """

    cmd_queue = Queue()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((ip_addr, port))
        receiver = Receiver(sock)
        sender = Sender(sock, cmd_queue)
        receiver.start()
        sender.start()
        while receiver.is_alive() and sender.is_alive():
            line = sys.stdin.readline().strip()
            if line:
                cmd_queue.put(line) 
            time.sleep(1/30)


if __name__ == "__main__":
    parser = ArgumentParser(
            prog="s3k-poc-client",
            description="Client for communicating with PoC applications")
    parser.add_argument('ip_addr', type=str)
    parser.add_argument('port', type=int)
    parser.add_argument('-f', '--frame-size', type=int, default=256)
    args = parser.parse_args()
    FRAME_SIZE = args.frame_size

    main(args.ip_addr, args.port)
