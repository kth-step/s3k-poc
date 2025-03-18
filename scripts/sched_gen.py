#!/usr/bin/env python3
import numpy as np
from scipy.optimize import minimize, differential_evolution, milp, Bounds
from typing import Tuple
from itertools import product

class LinearSchedule:
    def __init__(self, m: int, n: int, M_range: Tuple[int, int] = (-5, 5),
                 b_range: Tuple[int, int] = (0, 100), v_range: Tuple[int, int] = (0, 10)):
        """
        Initialize the LinearSchedule with given dimensions and ranges.

        Parameters:
        - m (int): Number of rows for matrix M.
        - n (int): Number of columns for matrix M.
        - M_range (Tuple[int, int]): Range for random values in matrix M.
        - b_range (Tuple[int, int]): Range for random values in vector b.
        - v_range (Tuple[int, int]): Range for random values in vector v.
        """
        self.m = m
        self.n = n

        # Validate ranges
        if any(not (isinstance(r, tuple) and len(r) == 2 and all(isinstance(i, int) for i in r)) for r in [M_range, b_range, v_range]):
            raise ValueError("Ranges must be tuples of two integers.")

        # Generate matrix M ensuring no row has all non-positive values
        while True:
            self.M = np.random.randint(M_range[0], M_range[1] + 1, size=(m, n))
            if np.all(np.max(self.M, axis=1) > 0):
                break

        self.b = np.random.randint(b_range[0], b_range[1] + 1, size=(m,))
        self.v_range = v_range

    def randsched(self) -> np.ndarray:
        """
        Generate a random schedule based on the initialized parameters.

        Returns:
        - s (np.ndarray): A flattened array representing the schedule.
        """
        v = np.random.randint(self.v_range[0], self.v_range[1] + 1, size=self.n)
        return self.sched(v)

    def sched(self, v: np.ndarray) -> np.ndarray:
        """
        Compute the schedule for a given vector v.

        Parameters:
        - v (np.ndarray): Input vector of shape (n, 1).

        Returns:
        - s (np.ndarray): A flattened array representing the schedule.

        Raises:
        - ValueError: If the input vector v is not within the specified range or has incorrect dimensions.
        """
#        if v.shape != (self.n,):
#            raise ValueError(f"Vector dimensions must be {self.n}.")
        if np.any((v < self.v_range[0]) | (v > self.v_range[1])):
            raise ValueError(f"Vector values must be in the range {self.v_range}.")

        s = self.M.dot(v) + self.b
        s[s < 0] = 0
        return s.flatten()

    def minimum_cycle(self) -> Tuple[np.ndarray, int]:
        """
        Find the vector v that maximizes the L1 norm of Mv + b.

        Returns:
        - v (np.ndarray): The vector v that maximizes the L1 norm.
        - l1_norm (int): The L1 norm value.
        """
        # Initial guess within bounds
        x0 = np.ones(self.n) * (self.v_range[0] + self.v_range[1]) / 2
        bounds = [(self.v_range[0], self.v_range[1])] * self.n

        # Use minimize to find the optimal v
        result = minimize(lambda v: -self.sched(v).sum(), x0, bounds=bounds)

        if result.success:
            v = np.round(result.x).astype(int)
            l1_norm = np.sum(self.sched(v))
            return v, l1_norm
        else:
            raise ValueError("Optimization failed: " + result.message)

    def component_wcet(self) -> np.ndarray:
        M = self.M.copy()
        M[M < 0] = 0
        v = np.ones(self.n, dtype=int) * self.v_range[1]
        return M.dot(v) + self.b


lin_sched = LinearSchedule(8, 10, v_range=(0,1))

print(lin_sched.M, lin_sched.b)
print(lin_sched.minimum_cycle())
print((lin_sched.component_wcet(),lin_sched.component_wcet().sum()))
