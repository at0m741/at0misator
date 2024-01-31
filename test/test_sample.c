#include <criterion/criterion.h>
#include "../inc/project.h"

Test(test2_suite, simple_sub_test) {
	cr_expect(1 == 1, "Expected 1 to be equal to 1");
}
