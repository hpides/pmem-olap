************************************************************************
file with basedata            : md283_.bas
initial value random generator: 337296647
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  20
horizon                       :  142
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     18      0       29       11       29
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           7  11
   3        3          2          10  14
   4        3          3           5   7   9
   5        3          3           6  13  17
   6        3          2           8  14
   7        3          2          13  19
   8        3          2          11  16
   9        3          3          10  12  16
  10        3          2          13  15
  11        3          2          12  15
  12        3          1          19
  13        3          1          18
  14        3          2          15  16
  15        3          2          18  19
  16        3          1          18
  17        3          1          20
  18        3          1          20
  19        3          1          20
  20        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     1       0    4    0    5
         2     8       8    0    9    0
         3     8       0    2   10    0
  3      1     1       0    3    0    7
         2     7       0    3    3    0
         3    10       4    0    0    7
  4      1     2       9    0    0    6
         2     4       0    4   10    0
         3     9       1    0    6    0
  5      1     2       0    8    0    4
         2     4       3    0    7    0
         3     6       0    7    7    0
  6      1     9       6    0    0   10
         2    10       6    0    9    0
         3    10       0    8    6    0
  7      1     3       0    5    2    0
         2     4       0    5    0    7
         3     5       0    5    0    4
  8      1     2       8    0    0    9
         2     4       7    0    0    9
         3     8       6    0    0    8
  9      1     7       5    0    0    4
         2     8       0    4    0    4
         3    10       0    1    5    0
 10      1     1       9    0    7    0
         2     7       9    0    0    8
         3     8       9    0    6    0
 11      1     2       3    0    0    6
         2     7       0    4    0    3
         3     8       0    2    0    2
 12      1     1       4    0    8    0
         2     4       0    2    0    7
         3     9       0    1    8    0
 13      1     3       0    8    0    6
         2     5       0    3    7    0
         3     6       3    0    6    0
 14      1     5       0    3    4    0
         2     7       6    0    0    7
         3    10       5    0    4    0
 15      1     7       9    0    3    0
         2     8       0    6    0    2
         3     8       9    0    0    2
 16      1     2       0    4    0    6
         2     4       8    0    0    5
         3     6       6    0    0    4
 17      1     3       0    3    0    2
         2     7       6    0   10    0
         3     8       6    0    7    0
 18      1     2       0    2    0    5
         2     2       6    0    0    5
         3     5       3    0    0    4
 19      1     4      10    0    0    3
         2     5       0    7    0    2
         3     8       0    7    8    0
 20      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   24   16   93  104
************************************************************************