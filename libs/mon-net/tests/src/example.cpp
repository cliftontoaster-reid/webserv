#include <criterion/criterion.h>

Test(suite_name, basic_test) {
  int x = 42;
  cr_assert_eq(x, 42);
}
