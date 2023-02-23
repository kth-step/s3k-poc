// See LICENSE file for copyright and license details.
#pragma once

//
#define PAYLOAD 0x80010000
#define NHART	4

/* Number of processes. */
#define NPROC 8

/* Number of capabilities per process */
#define NCAP 64

/* Number of time slices in a major frame. */
#define NSLICE 16

/* Number of ticks per quantum. */
/* TICKS_PER_SECOND defined in platform.h */
#define NTICK 100000

/* Number of scheduler ticks. */
#define NSLACK 10000

/* Number of communications channels */
#define NCHANNEL 8

#define NDEBUG

/* Uncomment to enable memory protection */
// #define MEMORY_PROTECTION
