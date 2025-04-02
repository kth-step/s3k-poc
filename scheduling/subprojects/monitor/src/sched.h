#pragma once

#define V_MIN 0
#define V_MAX 1
#define N_COMPONENTS 8
#define N_VARIABLES 4

static const int a_matrix[32] = {3, 0,	0, 4,  0, 0, 0, 0, 0, 0, -5,
				 0, -5, 0, -2, 0, 5, 3, 0, 0, 0, 0,
				 4, -3, 5, 0,  0, 0, 0, 0, 0, -3};
static const int b_vector[8] = {3, 5, 10, 14, 7, 13, 7, 7};

static inline void sched_calc(int v[N_VARIABLES], int s[N_COMPONENTS])
{
	for (int i = 0; i < N_COMPONENTS; ++i) {
		s[i] = b_vector[i];
		for (int j = 0; j < N_VARIABLES; ++j) {
			s[i] += v[j] * a_matrix[i * N_VARIABLES + j];
		}
	}
}
