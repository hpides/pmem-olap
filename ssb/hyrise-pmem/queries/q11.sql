select sum(lo_extendedprice * lo_discount) from lineorder, dat where lo_orderdate = d_datekey and d_year = 1993 and lo_discount > 0 and lo_discount < 4 and lo_quantity < 25;
