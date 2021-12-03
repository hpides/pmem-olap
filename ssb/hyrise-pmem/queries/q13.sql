select sum(lo_extendedprice * lo_discount) as revenue
	from lineorder, dat
	where lo_orderdate = d_datekey
	and d_weeknuminyear = 6
	and d_year = 1994
	and lo_discount > 4 and lo_discount < 8
	and lo_quantity > 35 and lo_quantity < 41;