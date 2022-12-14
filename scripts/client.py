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
            data = data.split(b"\0")
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

    def run(self):
        while True:
            data = self.cmd_queue.get(block=True)
            data = data.encode('ASCII')
            i = 0
            j = len(data)
            while i < j:
                # sock.send returns number of bytes sent,
                # we repeat until all data has been sent
                i += self.sock.send(data[i:j])
            self.log_queue.put("> Sent data")

class Interface(threading.Thread):
    def __init__(self, win, log_queue, cmd_queue):
        threading.Thread.__init__(self)
        self.win = win
        height, width = self.win.getmaxyx()
        output_height = height -2
        input_height = 1
        self.output_win = win.subwin(output_height, width, 0, 0)
        self.input_win = win.subwin(input_height, width, output_height, 0)
        self.input_win.keypad(True)
        self.input_win.nodelay(True)
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
        height, width = self.output_win.getmaxyx()
        updated = False
        try:
            while True:
                item = self.log_queue.get(block=False)
                updated = True
                self.outputs.append(item)
        except:
            pass
        if updated:
            self.outputs = self.outputs[-(height - 2):]
            self.output_win.clear()
            for i, line in enumerate(self.outputs):
                self.output_win.addstr(i+1, 1, line)
            self.output_win.border()
            self.output_win.refresh()

    def handle_input(self):
        try:
            key = self.input_win.getkey()
            if key == "\n":
                if self.input:
                    self.inputs.append(self.input)
                    self.cmd_queue.put(self.input + '\0')
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
        self.input_win.clear()
        self.input_win.addstr(0, 0, " > " + self.input)
        self.input_win.refresh()

    def run(self):
        self.output_win.border()
        self.input_win.addstr(0, 0, "> ")
        self.output_win.refresh()
        self.input_win.refresh()
        while True:
            self.handle_output()
            self.handle_input()
            time.sleep(1/30)


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
        interface.run()


if __name__ == "__main__":
    parser = ArgumentParser(
            prog="s3k-poc-client",
            description="Client for communicating with PoC applications")
    parser.add_argument('ip_addr', type=str)
    parser.add_argument('port', type=int)
    args = parser.parse_args()
    curses.wrapper(main, args.ip_addr, args.port)
