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
                self.log.put(">> " + byte.decode('ASCII').strip())

class Sender(threading.Thread):
    def __init__(self, sock, log_queue, cmd_queue):
        threading.Thread.__init__(self)
        self.sock = sock
        self.daemon = True
        self.name = "sender"
        self.log_queue = log_queue
        self.cmd_queue = cmd_queue

    def get_msg(self):
        while True:
            cmd = self.cmd_queue.get(block=True)
            try:
                args = shlex.split(cmd)
            except ValueError:
                return ('Invalid command', None)
            if args[0] == 'send' and len(args) == 2:
                return (f'{args[0]} "{args[1]}"', args[1])
            elif args[0] == 'load-binary' and len(args) == 2:
                file = args[1]
                return (f'{args[0]} {args[1]}', args[1])
            else:
                return ('Invalid command', None)


    def run(self):
        while True:
            (cmd, data) = self.get_msg()
            if data:
                self.sock.sendall(data.encode('ASCII') + b'\n')
            self.log_queue.put(f"> {cmd}")

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
            self.win.addstr(i, 1, line)

    def handle_input(self):
        try:
            key = self.win.getkey()
            if key == "\n":
                if self.input:
                    self.inputs.append(self.input)
                    self.cmd_queue.put(self.input + '\n')
                    self.input = ""
                    self.input_old = ""
                    self.input_idx = 0
            elif len(key) == 1:
                self.input += key
            elif key == "KEY_UP":
                if self.input_idx == 0:
                    self.input_old = self.input
                self.input_idx = min(self.input_idx+1, len(self.inputs)-1)
                if self.input_idx > 0:
                    self.input = self.inputs[-self.input_idx]
            elif key == "KEY_DOWN":
                self.input_idx = max(self.input_idx-1, 0)
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
