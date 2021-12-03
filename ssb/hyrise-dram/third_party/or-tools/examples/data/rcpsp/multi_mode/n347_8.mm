************************************************************************
file with basedata            : cn347_.bas
initial value random generator: 1107639675
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  136
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  3   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       28        8       28
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   7  10
   3        3          3          15  16  17
   4        3          3           5   6   7
   5        3          2           8   9
   6        3          3           8  10  11
   7        3          3           8   9  15
   8        3          1          14
   9        3          2          11  12
  10        3          2          12  15
  11        3          1          13
  12        3          1          13
  13        3          1          14
  14        3          2          16  17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2  N 3
------------------------------------------------------------------------
  1      1     0       0    0    0    0    0
  2      1     2       6   10    8    7    9
         2     4       4    7    5    6    9
         3     5       3    7    3    6    6
  3      1     3       4    8    8    3    4
         2     6       3    6    6    2    3
         3    10       3    2    4    1    2
  4      1     5       3    5    6    7    7
         2     7       2    5    4    7    4
         3    10       2    5    2    6    1
  5      1     1       7    5    5    7    7
         2     3       5    3    3    5    6
         3    10       4    3    3    5    5
  6      1     6       9    6    8    6    7
         2     7       9    4    5    4    7
         3    10       9    3    4    4    6
  7      1     2       7   10    7    7    8
         2     5       5   10    7    4    7
         3     9       5    9    6    3    7
  8      1     6       7    5    3    4    4
         2     7       6    4    3    4    2
         3    10       6    1    2    3    1
  9      1     6       3    5    9    6    5
         2     8       3    4    9    6    4
         3     9       2    4    9    5    4
 10      1     6       6    8    8    5   10
         2     7       6    6    8    4    5
         3    10       6    6    7    4    3
 11      1     3       1    4    4    6    8
         2     4       1    3    4    6    7
         3     8       1    2    3    5    6
 12      1     4       9    6    6    9    7
         2     6       7    6    6    8    6
         3    10       4    5    6    8    6
 13      1     3       6    7    4    5    9
         2     5       6    6    3    5    9
         3    10       6    6    2    5    9
 14      1     1       6    3   10    6    3
         2     4       5    2    7    6    3
         3     5       5    1    4    5    2
 15      1     1       8    7    6    9    8
         2     2       2    6    5    6    6
         3     2       1    6    3    7    2
 16      1     3       2    9    6    5    4
         2     3       3    9    5    5    4
         3     9       1    9    5    5    4
 17      1     2      10    4   10    4    6
         2     7       9    3   10    4    5
         3     9       8    1   10    3    1
 18      1     0       0    0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2  N 3
   20   20   91   85   86
************************************************************************