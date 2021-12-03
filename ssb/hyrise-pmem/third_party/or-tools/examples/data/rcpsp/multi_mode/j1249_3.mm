************************************************************************
file with basedata            : md113_.bas
initial value random generator: 1147109198
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  14
horizon                       :  96
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     12      0       25        1       25
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   8   9
   3        3          3           7   8   9
   4        3          3           6   8  10
   5        3          3           6  11  12
   6        3          1          13
   7        3          1          10
   8        3          3          11  12  13
   9        3          1          13
  10        3          2          11  12
  11        3          1          14
  12        3          1          14
  13        3          1          14
  14        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     3       0    6    6    9
         2     4       0    5    6    7
         3    10       0    5    5    4
  3      1     8       0    9    7    5
         2    10       0    3    4    4
         3    10      10    0    5    5
  4      1     3       0    8    8    7
         2     8       5    0    7    6
         3    10       0    5    7    4
  5      1     3       8    0    3    5
         2     4       0    4    2    4
         3     5       0    3    2    4
  6      1     2       6    0    3    5
         2     3       4    0    2    3
         3     5       1    0    1    2
  7      1     4       2    0    9   10
         2     5       0    5    9    9
         3     7       0    4    8    9
  8      1     1       3    0    8    9
         2     1       6    0    7    9
         3     3       0    9    3    3
  9      1     3       5    0    9   10
         2     9       0    5    8    9
         3     9       4    0    8    9
 10      1     6       7    0   10    7
         2     9       7    0    9    7
         3    10       7    0    9    6
 11      1     7       0    4    4    7
         2     8       0    3    4    7
         3     9       6    0    3    6
 12      1     3       0    5   10    8
         2     6       0    2    7    7
         3     8       8    0    2    7
 13      1     2       4    0    2    7
         2     9       0    1    2    5
         3    10       3    0    2    5
 14      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   11   10   73   83
************************************************************************