#!/usr/bin/gawk -f

BEGIN {
  print $0;
  printf "%-25s %5s %-20s\n",
      "Benchmark", "STL(ns)", "B-Tree(ns) @    <size>"
  printf "--------------------------------------------------------\n";
}

/^BM_/ {
  split($1, name, "_");
  if (name[2] == "stl") {
    stl = $2;
    printf "%-25s %5d    ", name[1] "_" name[3] "_" name[4] "_" name[5], stl;
    printf "%5d %+7.2f%%  <%3d>\n", btree, 100.0 * (stl - btree) / stl, btree_size;
    fflush();
  } else if (name[2] == "btree") {
    if (name[3] == 256) {
      btree = $2
      btree_size = name[3]
    }
  } else {
    printf "ERROR: %s unrecognized\n", $1
  }
}