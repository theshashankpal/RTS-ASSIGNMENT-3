RTS Assignment 3
Submitted by:- 
Name : Shashank Pal (2021H1030064G)
Name : Gaurav Saxena (2021H1030056G)
Name : Prayag Raj Sharma (2021H1030023G)
-----------------------------------------------------------------------------------------------------------------------
We have implemented the code in C language as we had taken permission for the same from you,Sir.
-----------------------------------------------------------------------------------------------------------------------
HOW TO COMPILE :

First run : make clean (to remove any unnecessary object and executable files)
Lastly run : make
-----------------------------------------------------------------------------------------------------------------------
HOW TO RUN :

To run program, type following with required amount of arguments : ./output <complete path of the input Task file>

For ex : ./output src/tasks1.txt

P.S.: For own set of tasks to run, the input task file should follow the below format.
<No. of task>
<Period of Task1> <Execution of Task 1> <Deadline of Task 1>
<Period of Task2> <Execution of Task 2> <Deadline of Task 2>
and so on

Also the program uses Linux multi-threading, so -lpthread library is required as a pre-requisite.

-----------------------------------------------------------------------------------------------------------------------
Sample set of Tasks used for testing

Test Case 1:

2 1 2
3 1.25 3.5
5 0.25 6
<Should be scheduable>

Test Case 2:

4 1.5 5
4 1 6
6 2 8
<Should be scheduable>


Test Case 3:

3 1 3
5 1.5 5
7 1.25 7
9 0.5 9
10 1 10
<Should NOT be scheduable>


Test Case 4:

100 20 100
150 40 150
350 100 350
<Should be scheduable>


Test Case 5:

3 1 3
5 1.5 5
7 1.25 7
9 0.5 9
<Should be scheduable>


Test Case 6:

2 2 2
3 3 3
4 4 4
<Should NOT be scheduable>
-----------------------------------------------------------------------------------------------------------------------