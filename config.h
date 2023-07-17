// See LICENSE file for copyright and license details.
#pragma once

/* Number of processes. */
#define NUM_OF_PROCESSES 8

/* Number of capabilities per process */
#define NUM_OF_CAPABILITIES 32 

/* Number of minor frames in a major frame. */
#define NUM_OF_FRAMES 32ull

/* Number of ticks per minor frame. */
#define FRAME_LENGTH ((unsigned long long) (TICKS_PER_SECOND / NUM_OF_FRAMES / 10ull))

/* Number of scheduler ticks. */
#define SLACK_LENGTH 10

/* Number of communications channels */
#define NUM_OF_CHANNELS 16

#define NDEBUG

/* Uncomment to enable memory protection */
#define MEMORY_PROTECTION
