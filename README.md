# MPI Programs

Q1. Write an MPI program to check if the input number is **prime**.
**Example Run:**
mpiexec -np 5 PrimalityTest
17

Q2. Write an MPI program that implements **Merge Sort**.
**Example Run:**
mpiexec -np 5 MS
9 5 1 2 4 7 5 3

Q3. Given coordinates of a convex polygon with n vertices, write an MPI program to calculate the **area of the polygon**.
**Note**: Input coordinates in clockwise/anti-clockwise direction
**Example Run:**
mpiexec -np 4 Area
4
0 2
2 2
2 0
0 0