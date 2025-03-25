#!/usr/bin/env python3
import numpy as np
import pprint
from scipy.optimize import milp, Bounds
from typing import Tuple
from itertools import product

class LinearSchedule:
    def __init__(self, m: int, n: int, A_range: Tuple[int, int] = (-5, 5),
                 b_range: Tuple[int, int] = (0, 10), v_range: Tuple[int, int] = (0, 1), p0 = 0.75):
        """
        Initialize the LinearSchedule with given dimensions and ranges.

        Parameters:
        - m (int): Number of rows for matrix A.
        - n (int): Number of columns for matrix A.
        - A_range (Tuple[int, int]): Range for random values in matrix A.
        - b_range (Tuple[int, int]): Range for random values in vector b.
        - v_range (Tuple[int, int]): Range for random values in vector v.
        """
        # Validate ranges
        if any(not (isinstance(r, tuple) and len(r) == 2 and all(isinstance(i, int) for i in r)) for r in [A_range, b_range, v_range]):
            raise ValueError("Ranges must be tuples of two integers.")

        # Generate matrix A ensuring no row has all non-positive values
        A = np.random.randint(A_range[0], A_range[1] + 1, size=(m, n))
        mask = np.random.rand(m,n) < p0
        A[mask] = 0
        b = np.zeros(m, dtype=int)
        for i in range(m):
            c = A[i,:]
            bounds = Bounds(lb=np.ones(n) * v_range[0], ub=np.ones(n) * v_range[1])
            integrality = np.ones(n,dtype=int)
            res = milp(c, integrality=integrality, bounds=bounds)
            b[i] = -c.dot(res.x)
        
        self.b = b + np.random.randint(b_range[0], b_range[1] + 1, size=(m,))
        self.v_range = v_range
        self.A = A

    def randsched(self) -> np.ndarray:
        """
        Generate a random schedule based on the initialized parameters.

        Returns:
        - s (np.ndarray): A flattened array representing the schedule.
        """
        m,n = self.A.shape
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
        m,n = self.A.shape
        if v.shape != (n,):
            raise ValueError(f"Vector dimensions must be {self.n}.")
        if np.any((v < self.v_range[0]) | (v > self.v_range[1])):
            raise ValueError(f"Vector values must be in the range {self.v_range}.")

        s = self.A.dot(v) + self.b
        return s.flatten().astype(int)

    def minimum_cycle(self) -> Tuple[np.ndarray, int]:
        """
        Find the vector v that maximizes the L1 norm of Av + b.

        Returns:
        - v (np.ndarray): The vector v that maximizes the L1 norm.
        - l1_norm (int): The L1 norm value.
        """
        m,n = self.A.shape
        c = -self.A.sum(axis=0)
        bounds = Bounds(lb=np.ones(n) * self.v_range[0], ub=np.ones(n) * self.v_range[1])
        integrality = np.ones(n,dtype=int)
        res = milp(c, integrality=integrality, bounds=bounds)
        s = self.sched(res.x)
        return s, s.sum()

    def component_wcet(self) -> np.ndarray:
        m,n = self.A.shape
        s = np.zeros(m, dtype=int)
        for i in range(m):
            c = -self.A[i,:]
            bounds = Bounds(lb=np.ones(n) * self.v_range[0], ub=np.ones(n) * self.v_range[1])
            integrality = np.ones(n,dtype=int)
            res = milp(c, integrality=integrality, bounds=bounds)
            s[i] = self.A[i].dot(res.x) + self.b[i]
        return (s, s.sum())

    def get_data(self):
        min_cycle = self.minimum_cycle()
        cmp_wcet = self.component_wcet()
        
        return {
                "A" : self.A,
                "b" : self.b,
                "v_range" : self.v_range,
                "min_cycle" : (min_cycle[0].tolist(), min_cycle[1]),
                "cmp_wcet" : (cmp_wcet[0].tolist(), cmp_wcet[1]),
        }

def numpy_to_c_array(np_array, array_name, data_type='int'):
    """
    Convert a NumPy array to a C-style array declaration.

    Parameters:
    - np_array: NumPy array to convert.
    - array_name: The name of the C array.
    - data_type: The data type of the C array (e.g., 'int', 'float', 'double').

    Returns:
    - A string representing the C-style array declaration.
    """
    # Flatten the array to handle multi-dimensional arrays
    flat_array = np_array.flatten()

    # Create the C array declaration
    c_array_declaration = f"{data_type} {array_name}[{np_array.size}] = {{"
    c_array_declaration += ", ".join(map(str, flat_array))
    c_array_declaration += "};"

    return c_array_declaration

if __name__ == "__main__":
    lin_sched = LinearSchedule(3, 4, v_range=(0,1), p0=0.5)
    data = lin_sched.get_data()
    A = data["A"]
    b = data["b"]
    v_min, v_max = data["v_range"]
    print("#pragma once")
    print()
    print(f"#define V_MIN {v_min}")
    print(f"#define V_MAX {v_max}")
    print(f"#define N_COMPONENTS {A.shape[0]}")
    print(f"#define N_VARIABLES {A.shape[1]}")
    print()
    print(numpy_to_c_array(A, "a_matrix", "static const int"))
    print(numpy_to_c_array(b, "b_vector", "static const int"))
    print(
"""
static inline void sched_calc(int v[N_VARIABLES], int s[N_COMPONENTS]) {
    for (int i = 0; i < N_COMPONENTS; ++i) {
        s[i] = b_vector[i];
        for (int j = 0; j < N_VARIABLES; ++j) {
            s[i] += v[j] * a_matrix[i * N_VARIABLES + j];
        }
    }
}
"""
)
