#!/usr/bin/env python3
import numpy as np
import pprint
from scipy.optimize import milp, Bounds
from typing import Tuple
from itertools import product

class LinearSchedule:
    def __init__(self, m: int, n: int, M_range: Tuple[int, int] = (-5, 5),
                 b_range: Tuple[int, int] = (0, 10), v_range: Tuple[int, int] = (0, 1), p0 = 0.75):
        """
        Initialize the LinearSchedule with given dimensions and ranges.

        Parameters:
        - m (int): Number of rows for matrix M.
        - n (int): Number of columns for matrix M.
        - M_range (Tuple[int, int]): Range for random values in matrix M.
        - b_range (Tuple[int, int]): Range for random values in vector b.
        - v_range (Tuple[int, int]): Range for random values in vector v.
        """
        # Validate ranges
        if any(not (isinstance(r, tuple) and len(r) == 2 and all(isinstance(i, int) for i in r)) for r in [M_range, b_range, v_range]):
            raise ValueError("Ranges must be tuples of two integers.")

        # Generate matrix M ensuring no row has all non-positive values
        M = np.random.randint(M_range[0], M_range[1] + 1, size=(m, n))
        mask = np.random.rand(m,n) < p0
        M[mask] = 0
        b = np.zeros(m, dtype=int)
        for i in range(m):
            c = M[i,:]
            bounds = Bounds(lb=np.ones(n) * v_range[0], ub=np.ones(n) * v_range[1])
            integrality = np.ones(n,dtype=int)
            res = milp(c, integrality=integrality, bounds=bounds)
            b[i] = -c.dot(res.x)
        
        self.b = b + np.random.randint(b_range[0], b_range[1] + 1, size=(m,))
        self.v_range = v_range
        self.M = M

    def randsched(self) -> np.ndarray:
        """
        Generate a random schedule based on the initialized parameters.

        Returns:
        - s (np.ndarray): A flattened array representing the schedule.
        """
        m,n = self.M.shape
        v = np.random.randint(self.v_range[0], self.v_range[1] + 1, size=self.n)
        return self.sched(v).astype(int)

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
        m,n = self.M.shape
        if v.shape != (n,):
            raise ValueError(f"Vector dimensions must be {self.n}.")
        if np.any((v < self.v_range[0]) | (v > self.v_range[1])):
            raise ValueError(f"Vector values must be in the range {self.v_range}.")

        s = self.M.dot(v) + self.b
        return s.flatten().astype(int)

    def minimum_cycle(self) -> Tuple[np.ndarray, int]:
        """
        Find the vector v that maximizes the L1 norm of Mv + b.

        Returns:
        - v (np.ndarray): The vector v that maximizes the L1 norm.
        - l1_norm (int): The L1 norm value.
        """
        m,n = self.M.shape
        c = -self.M.sum(axis=0)
        bounds = Bounds(lb=np.ones(n) * self.v_range[0], ub=np.ones(n) * self.v_range[1])
        integrality = np.ones(n,dtype=int)
        res = milp(c, integrality=integrality, bounds=bounds)
        s = self.sched(res.x)
        return s, s.sum()

    def component_wcet(self) -> np.ndarray:
        m,n = self.M.shape
        s = np.zeros(m, dtype=int)
        for i in range(m):
            c = -self.M[i,:]
            bounds = Bounds(lb=np.ones(n) * self.v_range[0], ub=np.ones(n) * self.v_range[1])
            integrality = np.ones(n,dtype=int)
            res = milp(c, integrality=integrality, bounds=bounds)
            s[i] = self.M[i].dot(res.x) + self.b[i]
        return (s, s.sum())

    def get_data(self):
        min_cycle = self.minimum_cycle()
        cmp_wcet = self.component_wcet()
        
        return {
                "M" : self.M.tolist(),
                "b" : self.b.tolist(),
                "v_range" : self.v_range,
                "min_cycle" : (min_cycle[0].tolist(), min_cycle[1]),
                "cmp_wcet" : (cmp_wcet[0].tolist(), cmp_wcet[1]),
        }

if __name__ == "__main__":
    lin_sched = LinearSchedule(8, 8, v_range=(0,3), p0=0.7)
    pprint.pp(lin_sched.get_data())
