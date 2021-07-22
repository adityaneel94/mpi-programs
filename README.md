# MPI Programs

Q1. MPI program to check if the input number is **prime**.  
**Example Run:**  
mpiexec -np 5 ./PrimalityTest  
17  

Q2. MPI program that implements **Merge Sort**.  
**Example Run:**  
mpiexec -np 5 ./MS  
9 5 1 2 4 7 5 3  

Q3. MPI program to calculate the **area of the polygon**, given coordinates of a convex polygon with n vertices  
**Note**: Input coordinates in clockwise/anti-clockwise direction  
**Example Run:**  
mpiexec -np 4 ./Area  
4  
0 2  
2 2  
2 0  
0 0  

Q4. Implementation of a distributed algorithm in MPI to find the **Minimum Spanning Tree** of an undirected graph.  
**Example Run:**
mpirun -np 4 ./mst  
5 5  
0 1 2  
0 3 6  
1 3 8  
1 2 3  
1 4 5  
2 4 7  
3 4 9  

Q5. **Vertex coloring problem** 
Given an undirected graph G, assign a color to every vertex such that no two adjacent vertices have the same color. Total number of colors should not exceed d + 1 where d is the maximum degree of the graph. MPI program to find the optimal coloring to given graph G. The processes should communicate using asynchronous message passing.  
**Example Run:**
mpirun -np 4 ./vc  
6 7  
0 1  
0 2  
1 3  
2 3  
3 4  
3 5  
4 5  

Q6. MPI Distributed program to solve the **Stable Marriage Problem**. The processes should communicate using asynchronous message passing. Log the trace of events (pairing, breaking), as they happen, in a text file named as Log.txt.  
**Example Run:**  
mpirun -np 4 ./smp > Log.txt   
4  
3, 1, 2, 0  
1, 0, 2, 3  
0, 1, 2, 3  
0, 1, 2, 3  
0, 1, 2, 3  
0, 1, 2, 3  
0, 1, 2, 3  
0, 1, 2, 3  

Q7. **Walks in a graph** Given a directed graph, MPI program to count all possible walks (both closed and open) of length k from every source and destination pairs.  
**Example Run:**  
mpirun -np 4 ./walks  
4 5 2  
1 2
1 3
1 4
3 4
2 4

Q8. **4-cycles in a Graph** Given an undirected graph G, MPI program to calculate the total number of 4-vertex cycles in G.    
**Example Run:**       
mpirun -np 4 ./walks    
5 7  
0 1  
0 2  
1 2   
1 3  
2 3  
1 4  
2 4  
