select sum(lo_extendedprice * lo_discount) as revenue
	from lineorder, dat
	where lo_orderdate = d_datekey
	and d_yearmonthnum = 199401
	and lo_discount > 3 and lo_discount < 7
	and lo_quantity > 25 and lo_quantity < 36;