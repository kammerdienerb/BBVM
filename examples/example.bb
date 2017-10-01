entry:
	%0 =	getglobal	0
	arg	%0
	%1 =	fficall	%puts
	%2 =	alloc	8
	storei	%2,	0
	%3 =	load	%2
	brc	%3	%exit
	%4 =	alloc	8
	%5 =	alloc	8
	fstorei	%4,	1.234500
	fstorei	%5,	5.000000
	%6 =	fload	%4
	%7 =	faddi	%6,	4.567800
	%8 =	fmuli	%7,	10.100000
	%9 =	fload	%5
	%10 =	fdiv	%8,	%9
	fprint	%10
	%11 =	fequi	%9,	5.000000
	print	%11
	%12 =	fequi	%9,	6.000000
	print	%12
exit:
	escape
