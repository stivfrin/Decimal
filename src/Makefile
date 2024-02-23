FLAGS = -Wall -Wextra -Werror -std=c11
LFLAGS = -lcheck -lm -lpthread
SOURCES=s21_*.c
OBJECTS=s21_*.o

ifeq ($(shell uname -s), Linux)
	LFLAGS = -lm -lpthread -lcheck -lsubunit
endif

all: clean s21_decimal.a

test: s21_decimal.a
	gcc $(FLAGS) -c test.c
	gcc test.o $(FLAGS) $(OBJECTS) $(LFLAGS) -L. s21_decimal.a -o test
	rm -rf *.o
	
s21_decimal.a:
	gcc $(FLAGS) -c $(SOURCES)
	ar rc s21_decimal.a $(OBJECTS)
	ranlib s21_decimal.a

gcov_report: clean add_coverage test
	./test
	lcov -t "test" -o test.info -c -d .
	genhtml -o report test.info
	open report/index.html

clean:
	rm -rf *.o gcov_report *.a *.gcda *.gcno test test.info

style:
	clang-format -i *.c *.h -style=Google

add_coverage:
	$(eval FLAGS += --coverage)
