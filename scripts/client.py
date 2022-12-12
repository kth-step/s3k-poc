#!/usr/bin/env python3
"""
S3K Proof-of-Concept client application
"""

from argparse import ArgumentParser
from queue import Queue
import curses
import sys
import threading
import socket


def receiver(sock, queue):
    line = b""
    while True:
        data = sock.recv(1)
        if data == b'\n':
            queue.put(line) 
            line = b""
        else:
            line += data
        

def sender(tnet, queue):
    while True:
        msg = queue.get()
        tnet.write(msg.encode('utf-8'))


def tui(win, in_queue, out_queue):
    lines = []
    while True:
        item = in_queue.get()
        win.clear()
        lines.append(item)
        for i, resp in enumerate(lines[-(curses.LINES - 1):]):
            win.addstr(i, 0, resp)
        win.refresh()


def main(stdscr, ip_addr, port):
    """
    Main function of this module
    """
    stdscr.clear()
    stdscr.nodelay(True)
    curses.echo()
    in_queue = Queue()
    out_queue = Queue()

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip_addr, port))

    thread = threading.Thread(target=receiver, args=(sock, in_queue,))
    thread.start()
    tui(stdscr, in_queue, out_queue)


parser = ArgumentParser(
        prog="s3k-poc-client",
        description="Client for communicating with PoC applications")
parser.add_argument('ip_addr', type=str)
parser.add_argument('port', type=int)
args = parser.parse_args()
curses.wrapper(main, args.ip_addr, args.port)
