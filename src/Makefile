CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -g
COVFLAGS = --coverage


TARGET = main_test_unit
LIB = s21_decimal.a
SRC = s21_helper_functions.c s21_div.c s21_add.c s21_floor.c s21_from_decimal_to_float.c s21_from_decimal_to_int.c s21_from_int_to_decimal.c s21_is_equal.c s21_is_greater_or_equal.c s21_is_greater.c s21_is_less_or_equal.c s21_is_less.c s21_is_not_equal.c s21_mul.c s21_negate.c s21_round.c s21_sub.c s21_truncate.c s21_from_float_to_decimal.c
TEST_SRC = $(wildcard s21_test/*.c)
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
DEPS = s21_decimal.h s21_helper_functions.h
LDFLAGS = -lcheck -lm -lpthread -lrt -lsubunit

.PHONY: all clean re test gcov_report style mem help

all: $(LIB) $(TARGET)


$(LIB): $(OBJ)
	ar rcs $@ $^


$(TARGET): $(TEST_OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@


test: clean all
	./$(TARGET)


gcov_report: clean
	$(MAKE) CFLAGS="$(CFLAGS) $(COVFLAGS)" all
	./$(TARGET)
	gcovr -r . --exclude="s21_test/" --html --html-details -o coverage.html
	open coverage.html 2>/dev/null


clean:
	rm -f $(TARGET) *.o *.gcno *.gcda *.gcov $(LIB) coverage*.html coverage*.css s21_test/*.o s21_test/*.gcda s21_test/*.gcno

re: clean all


style:
	clang-format -style=Google -n *.c *.h -n s21_test/*.c s21_test/*.h 
	clang-format -style=Google -i *.c *.h -i s21_test/*.c s21_test/*.h 

mem: re 
	valgrind --tool=memcheck --leak-check=yes  ./main_test_unit

help:
	@echo "Доступные цели:"
	@echo "  all         - Сборка проекта"
	@echo "  clean       - Очистка"
	@echo "  test        - Запуск тестов"
	@echo "  gcov_report - Отчёт покрытия"
	@echo "  mem         - Тест на утечки памяти"
	@echo "  style       - Проверка на clang-format и форматировние стиля в случае необходимости"
	@echo "  re          - Пересборка проекта"
	@echo "  help        - Данная справка"

.DEFAULT:
	@$(MAKE) help