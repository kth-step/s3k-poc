#!/usr/bin/env python3

import numpy as np
from scipy.optimize import milp, LinearConstraint, Bounds


done = False
while not done:
    M = np.random.randint(-5, 6, size = (3,3,))
    B = np.random.randint(0, 2, size = (3,3,))
    M[B == 1] = 0
    done = True
    for i in range(3):
        if not np.any(M[i,:] > 0):
            done = False
            
M *= 100

print("M = ", M)

def s3k_wcmf(M):
    v_opt = np.array([0,0,0])
    opt = 0

    for i in range(2):
        for j in range(2):
            for k in range(2):
                v = np.array([i,j,k])
                s = M.dot(v)
                s[s < 0] = 0
                if opt < np.sum(s):
                    v_opt = v
                    opt = np.sum(s)
    return v_opt


# Example usage:
v = s3k_wcmf(M)
print("Vector v that maximizes the L1 norm of Mv:", v)
s = M.dot(v)
s[s<0] = 0
print("Worst-case major frame (S3K): ", np.sum(s))

T = M.copy()
T[T < 0] = 0
wcet_traditional = 0
for i in range(3):
    wcet_traditional += np.sum(T[i,:] * 1)

print("Worst-case major frame (traditional): ", wcet_traditional)

print("Gain: ", (wcet_traditional - np.sum(s)) / wcet_traditional * 100)
