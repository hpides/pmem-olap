************************************************************************
file with basedata            : mf31_.bas
initial value random generator: 304319611
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  32
horizon                       :  246
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     30      0       43       21       43
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          2           6  13
   3        3          3           5   6   7
   4        3          2          14  25
   5        3          2           8  22
   6        3          1          11
   7        3          3           9  10  30
   8        3          3          13  15  18
   9        3          2          12  15
  10        3          2          18  28
  11        3          3          15  17  19
  12        3          3          16  18  28
  13        3          2          20  30
  14        3          3          21  27  28
  15        3          2          16  20
  16        3          2          21  23
  17        3          1          26
  18        3          1          31
  19        3          2          21  24
  20        3          2          24  26
  21        3          1          31
  22        3          2          29  30
  23        3          2          24  26
  24        3          1          25
  25        3          2          27  29
  26        3          1          27
  27        3          1          31
  28        3          1          29
  29        3          1          32
  30        3          1          32
  31        3          1          32
  32        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     5       8    5    5    0
         2     6       6    2    0    2
         3    10       5    1    4    0
  3      1     3       3    6    3    0
         2     3       3    5    0   10
         3     9       2    3    0    4
  4      1     1       9   10    0    4
         2     5       8    6    9    0
         3     8       6    5    6    0
  5      1     4      10    5    6    0
         2     7       8    3    6    0
         3     8       7    2    0    6
  6      1     2       7    6    8    0
         2     7       7    6    7    0
         3    10       4    5    0    4
  7      1     3       4    2    0    7
         2     5       3    1    3    0
         3    10       3    1    0    6
  8      1     4       9    7   10    0
         2    10       4    5    7    0
         3    10       4    6    0    6
  9      1     1       7    8    6    0
         2     9       5    1    0    3
         3     9       6    3    5    0
 10      1     2       8    9    6    0
         2     6       6    7    0    3
         3     9       2    7    5    0
 11      1     3       2    4    0    5
         2     7       2    4    3    0
         3     7       2    4    0    3
 12      1     1       7    5    0    4
         2     3       6    4    0    2
         3     3       5    4    6    0
 13      1     2       5   10    0    8
         2     8       5    6    0    7
         3     9       3    3    0    4
 14      1     5       7   10    1    0
         2     8       5    9    0    7
         3    10       4    9    0    4
 15      1     1       4    6    0    3
         2     2       2    4    1    0
         3     6       1    4    0    3
 16      1     6       7    2    9    0
         2     7       5    2    0    8
         3     8       4    1    9    0
 17      1     3       4    5    0    3
         2     9       4    5    0    1
         3     9       3    4    9    0
 18      1     1       8   10    0    5
         2     5       6    9    0    3
         3     7       6    8    0    2
 19      1     5       4    8   10    0
         2     6       4    7   10    0
         3     8       4    6   10    0
 20      1     1       3    6    7    0
         2     4       3    4    0    7
         3     8       3    3    7    0
 21      1     1       8    7    7    0
         2     4       5    6    0    6
         3     5       5    5    0    5
 22      1     2       4    9    7    0
         2     5       4    9    4    0
         3    10       2    9    0    6
 23      1     5       5    3    0    8
         2     5       6    4    2    0
         3     8       5    2    0    9
 24      1     3       3   10    0    5
         2     5       2    9    5    0
         3     8       2    9    0    4
 25      1     6       4    3    0    7
         2     9       3    3    1    0
         3     9       2    2    0    7
 26      1     5      10    7   10    0
         2     7       8    4    0    6
         3     7       7    4   10    0
 27      1     7       8    3    6    0
         2     8       5    3    5    0
         3    10       3    2    2    0
 28      1     5       8    3    7    0
         2     9       8    2    3    0
         3    10       7    2    0    7
 29      1     3       6    6    6    0
         2     4       6    1    0    6
         3     4       6    2    6    0
 30      1     4       8    2    7    0
         2     6       5    1    0    6
         3     9       4    1    0    5
 31      1     4       8    6    0    3
         2     4       9    8    0    2
         3     8       6    5    9    0
 32      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   27   34  169  156
************************************************************************