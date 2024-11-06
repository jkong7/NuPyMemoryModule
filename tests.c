/*tests.c*/

//
// << DESCRIPTION >>
//
// << YOUR NAME >>
//
// Initial template: Prof. Joe Hummel
// Northwestern University
// CS 211
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ram.h"
#include "gtest/gtest.h"

//
// private helper functions:
//


//
// some provided unit tests to get started:
//
TEST(memory_module, initialization)
{
  //
  // create a new memory and make sure it's initialized properly:
  //
  struct RAM* memory = ram_init();

  ASSERT_TRUE(memory != NULL);        // use ASSERT_TRUE with pointers
  ASSERT_TRUE(memory->cells != NULL);

  ASSERT_EQ(memory->num_values, 0);  // use ASSERT_EQ for comparing values
  ASSERT_EQ(memory->capacity, 4);

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}

TEST(memory_module, read_by_name_does_not_exist) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // read a var that doesn't exist:
  //
  struct RAM_VALUE* value = ram_read_cell_by_name(memory, "x");
  ASSERT_TRUE(value == NULL);  // use ASSERT_TRUE with pointers

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}

TEST(memory_module, write_one_int) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  //
  // store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  //
  // now check the memory, was x = 123 stored properly?
  //
  ASSERT_EQ(memory->num_values, 1);
  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].value.types.i, 123);
  ASSERT_STREQ(memory->cells[0].identifier, "x");  // strings => ASSERT_STREQ

  //
  // tests passed, free memory
  //
  ram_destroy(memory);
}


//
// TODO: add lots more unit tests
//


