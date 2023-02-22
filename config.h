// See LICENSE file for copyright and license details.
#pragma once

#define PAYLOAD 0x80010000
#define NHART	4

/* Number of processes. */
#define NPROC 6

/* Number of capabilities per process */
#define NCAP 64

/* Number of time slices in a major frame. */
#define NSLICE 32

/* Number of communications channels */
#define NPORT 8

/* Number of ticks per quantum. */
/* TICKS_PER_SECOND defined in platform.h */
#define NTICK 100000

/* Number of scheduler ticks. */
#define NSLACK 10000

#define NDEBUG

/* Uncomment to enable memory protection */
// #define MEMORY_PROTECTION
