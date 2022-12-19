#!/usr/bin/env python3
"""
S3K Proof-of-Concept client application
"""

from argparse import ArgumentParser
from queue import Queue, Empty as QueueEmpty
import curses
import threading
import socket
import time
import sys
import shlex
import string

def ppp_encode(data):
    data = bytearray(data)
    data = data.replace(b"\\", b"\\|")
    data = data.replace(b"\n", b"\\*")
    return b"\n" + data + b"\n"

class Receiver(threading.Thread):
    def __init__(self, sock, log_queue):
        threading.Thread.__init__(self)
        self.sock = sock
        self.daemon = True
        self.name = "receiver"
        self.log = log_queue

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
            for byte in data[0:-1]:
                text = bytes([i if i in bytes(string.printable, 'ascii') else 46 for i in byte])
                text = text.replace(b'\\|', b'\\')
                text = text.replace(b'\\*', b'\\n')
                if byte:
                    self.log.put(text)

class Sender(threading.Thread):
    def __init__(self, sock, log_queue, cmd_queue):
        threading.Thread.__init__(self)
        self.sock = sock
        self.daemon = True
        self.name = "sender"
        self.log_queue = log_queue
        self.cmd_queue = cmd_queue

    def parse_command(self,cmd):
        args = shlex.split(cmd)
        if args[0] == 'send' and len(args) == 3 and len(args[1]) <= 8:
            receiver = args[1].rjust(8).encode('ascii')
            data = args[2].encode('ascii')
            return receiver + b":" + data
        elif args[0] == 'send-file' and len(args) == 3 and len(args[1]) <= 8:
            receiver = args[1].rjust(8).encode('ascii')
            filename = args[2]
            with open(filename, 'rb') as file:
                data = file.read()
            return receiver + b":" + data
        else:
            return None

    def run(self):
        while True:
            try:
                cmd = self.cmd_queue.get(block=True)
                self.log_queue.put(f"> {cmd}")
                data = self.parse_command(cmd)
                if data:
                    self.sock.sendall(ppp_encode(data))
                else:
                    self.log_queue.put("Invalid command")
            except Exception as e:
                self.log_queue.put(f"{e}")


class Interface:
    def __init__(self, win, log_queue, cmd_queue):
        self.win = win
        self.win.keypad(True)
        self.win.nodelay(True)
        self.log_queue = log_queue
        self.cmd_queue = cmd_queue
        self.daemon = False
        self.name = "interface"
        self.outputs = []
        self.inputs = []
        self.input = ""
        self.input_old = None
        self.input_idx = 0

    def handle_output(self):
        while True:
            try:
                item = self.log_queue.get(block=False)
                self.outputs.append(item)
            except QueueEmpty:
                break
        height, width = self.win.getmaxyx()
        self.outputs = self.outputs[-(height - 3):]
        for i, line in enumerate(self.outputs):
            self.win.addstr(i, 1, line[:width-1])

    def handle_input(self):
        try:
            key = self.win.getkey()
            if key == "\n":
                if self.input:
                    self.inputs.append(self.input)
                    self.cmd_queue.put(self.input)
                    self.input = ""
                    self.input_old = ""
                    self.input_idx = 0
            elif len(key) == 1:
                self.input += key
            elif key == "KEY_UP":
                if self.input_idx == 0:
                    self.input_old = self.input
                self.input_idx = min(self.input_idx+1, len(self.inputs))
                if self.input_idx > 0:
                    self.input = self.inputs[-self.input_idx]
            elif key == "KEY_DOWN":
                self.input_idx = max(self.input_idx, 0)
                if self.input_idx == 0:
                    self.input = self.input_old
                else:
                    self.input = self.inputs[-self.input_idx]
            elif key == "KEY_BACKSPACE":
                self.input = self.input[0:len(self.input)-1]
        except:
            pass
        height, width = self.win.getmaxyx()
        self.win.addstr(height-2, 0, "-" * (width-1))
        self.win.addstr(height-1, 0, " > " + self.input)

    def update(self):
        self.win.clear()
        self.handle_output()
        self.handle_input()
        self.win.refresh()


def main(stdscr, ip_addr, port):
    """
    Main function of this module
    """

    curses.noecho()

    log_queue = Queue()
    cmd_queue = Queue()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((ip_addr, port))
        receiver = Receiver(sock, log_queue)
        sender = Sender(sock, log_queue, cmd_queue)
        interface = Interface(stdscr, log_queue, cmd_queue)
        receiver.start()
        sender.start()
        while receiver.is_alive() and sender.is_alive():
            interface.update()
            time.sleep(1/30)


if __name__ == "__main__":
    parser = ArgumentParser(
            prog="s3k-poc-client",
            description="Client for communicating with PoC applications")
    parser.add_argument('ip_addr', type=str)
    parser.add_argument('port', type=int)
    args = parser.parse_args()
    curses.wrapper(main, args.ip_addr, args.port)
