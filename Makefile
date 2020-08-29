test : test.cpp coctx.cpp coctx_swap.S  coroutine.cpp scheduler.cpp
	g++ test.cpp coctx.cpp coctx_swap.S  coroutine.cpp scheduler.cpp -g

clean :
	rm a.out