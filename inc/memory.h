#pragma once
struct memory {
	char kernel[0x10000];
	char boot[0x10000];
	char monitor[0x8000];
	char crypto[0x8000];
	char uartppp[0x8000];
	char app0[0x8000];
	char app1[0x8000];
	char shared[0x8000];
};

