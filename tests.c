/*tests.c*/

//
// Test suite for the memory module built in ram.c. Things to check include resizing, writing and reading, one off addresses, initialization, 
// num_values and capacity correctness. Other edge cases include string ram_value cases, handling immediately-after-double write and reads, and invalid
// cases (such as address out of bounds or getting address by name where a cell with that name has not been created)
//
// Jonathan Kong
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


//1. Resizing test: Test ram_write_cell_by_name is valid THROUGH resizing operation and that resizing changes the fields appropriately 
TEST(memory_module, resize_ram) {
  struct RAM* memory = ram_init(); 

  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 1;
  ram_write_cell_by_name(memory, i, "a");
  i.types.i = 2;
  ram_write_cell_by_name(memory, i, "b");
  i.types.i = 3;
  ram_write_cell_by_name(memory, i, "c");
  i.types.i = 4;
  ram_write_cell_by_name(memory, i, "d");

  ASSERT_EQ(memory->num_values, 4);
  ASSERT_EQ(memory->cells[0].value.types.i, 1);
  ASSERT_EQ(memory->cells[1].value.types.i, 2);
  ASSERT_EQ(memory->cells[2].value.types.i, 3);
  ASSERT_EQ(memory->cells[3].value.types.i, 4);

  ASSERT_TRUE(memory!=NULL); 

  i.types.i = 5;
  ram_write_cell_by_name(memory, i, "e");
  ASSERT_EQ(memory->num_values, 5);
  ASSERT_EQ(memory->capacity, 8);

  ASSERT_EQ(memory->cells[0].value.types.i, 1);
  ASSERT_EQ(memory->cells[1].value.types.i, 2);
  ASSERT_EQ(memory->cells[2].value.types.i, 3);
  ASSERT_EQ(memory->cells[3].value.types.i, 4);
  ASSERT_EQ(memory->cells[4].value.types.i, 5);
  ASSERT_EQ(strcmp(memory->cells[4].identifier, "e"), 0); 
  ASSERT_STREQ(memory->cells[4].identifier, "e"); 

  i.types.i=6; 
  ram_write_cell_by_name(memory, i, "f"); 
  ASSERT_EQ(memory->cells[5].value.types.i, 6); 
  i.types.i=7; 
  ram_write_cell_by_name(memory, i, "g"); 
  i.types.i=8; 
  ram_write_cell_by_name(memory, i, "h"); 
  ASSERT_EQ(memory->num_values, 8); 
  i.types.i=9; 
  ram_write_cell_by_name(memory, i, "i");
  ASSERT_EQ(memory->capacity, 16);
  

  ram_destroy(memory);
}

//2. Just basic writing and then reading 
TEST(memory_module, write_and_read_various_types) {
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE real_value;
    real_value.value_type = RAM_TYPE_REAL;
    real_value.types.d = 3.14;
    ram_write_cell_by_name(memory, real_value, "pi");

    struct RAM_VALUE boolean_value;
    boolean_value.value_type = RAM_TYPE_BOOLEAN;
    boolean_value.types.i = true;
    ram_write_cell_by_name(memory, boolean_value, "flag");

    struct RAM_VALUE* pi_copy = ram_read_cell_by_name(memory, "pi");
    ASSERT_TRUE(pi_copy != NULL);
    ASSERT_EQ(pi_copy->value_type, RAM_TYPE_REAL);
    ASSERT_EQ(pi_copy->types.d, 3.14);
    ram_free_value(pi_copy);

    struct RAM_VALUE* flag_copy = ram_read_cell_by_name(memory, "flag");
    ASSERT_TRUE(flag_copy != NULL);
    ASSERT_EQ(flag_copy->value_type, RAM_TYPE_BOOLEAN);
    ASSERT_EQ(flag_copy->types.i, true);
    ram_free_value(flag_copy);

    ram_destroy(memory);
}

//3. ram_write_cell_by_name should be able to overwrite an existing cell with the same name 
TEST(memory_module, overwrite_existing_variable) {
    struct RAM* memory = ram_init();
    
    struct RAM_VALUE i;
    i.value_type = RAM_TYPE_INT;
    i.types.i = 10;
    ram_write_cell_by_name(memory, i, "x");

    i.types.i = 20; 
    ram_write_cell_by_name(memory, i, "x");

    ASSERT_EQ(memory->num_values, 1);
    ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_INT);
    ASSERT_EQ(memory->cells[0].value.types.i, 20); 

    ram_destroy(memory);
}

// 4. Strings come with some more complexity because of their manual alloc and dealloc of memory - test basic write string 
TEST(memory_module, string_write) {
  struct RAM* memory = ram_init(); 
  struct RAM_VALUE s; 
  s.value_type = RAM_TYPE_STR;

  s.types.s = (char*)malloc(strlen("string") + 1);
  strcpy(s.types.s, "string");

  ram_write_cell_by_name(memory, s, "first");

  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_STR);
  ASSERT_STREQ(memory->cells[0].value.types.s, "string");

  free(s.types.s);
  ram_destroy(memory); 
}

//5. Some more string reading and writing
TEST(memory_module, more_string_write_read) {
    struct RAM* memory = ram_init();
    ASSERT_TRUE(memory != NULL);

    struct RAM_VALUE s;
    s.value_type = RAM_TYPE_STR;
    s.types.s = strdup("hello");

    bool result = ram_write_cell_by_name(memory, s, "test");
    ASSERT_TRUE(result);

    struct RAM_VALUE* read_value = ram_read_cell_by_name(memory, "test");
    ASSERT_TRUE(read_value != NULL);
    ASSERT_EQ(read_value->value_type, RAM_TYPE_STR);
    ASSERT_TRUE(strcmp(read_value->types.s, "hello") == 0);

    free(s.types.s);
    ram_free_value(read_value);
    ram_destroy(memory);
}

//6. Some more resizing - loop through 20 time so plenty more resizes - create checkpoints at 10 and 20
TEST(memory_module, memory_resize_and_check) {
    struct RAM* memory = ram_init();
    struct RAM_VALUE s;
    s.value_type = RAM_TYPE_STR;
    for (int i = 0; i < 10; i++) {
        char name[10];
        sprintf(name, "key%d", i);

        char* value = (char*)malloc(20);
        sprintf(value, "value%d", i);
        s.types.s = value;
        ram_write_cell_by_name(memory, s, name);
        free(value);
    }

    ASSERT_EQ(memory->num_values, 10);
    ASSERT_EQ(memory->capacity, 16);

    for (int i = 0; i <10; i++) {
        char name[10];
        sprintf(name, "key%d", i);
        struct RAM_VALUE* read_value = ram_read_cell_by_name(memory, name);
        ASSERT_TRUE(read_value != NULL);
        ASSERT_EQ(read_value->value_type, RAM_TYPE_STR);

        char expected[20];
        sprintf(expected, "value%d", i);
        ASSERT_TRUE(strcmp(read_value->types.s, expected) == 0);

        ram_free_value(read_value);
    }

    ram_destroy(memory);
}

//7. Writing edge cases - no address exists there || address -1, read and write should both handle these cases 
TEST(memory_module, write_to_invalid) {
    struct RAM* memory = ram_init();

    struct RAM_VALUE i;
    i.value_type = RAM_TYPE_INT;
    i.types.i = 99;

    bool res = ram_write_cell_by_addr(memory, i, 0); 
    ASSERT_EQ(res, false);

    res = ram_write_cell_by_addr(memory, i, -1); 
    ASSERT_EQ(res, false);

    struct RAM_VALUE* val = ram_read_cell_by_addr(memory, -1);
    ASSERT_TRUE(val== NULL);

    ram_free_value(val);
    ram_destroy(memory);
}

//8. Off by one! Lets write 4 cells, so at index 4 (one to the right), reading there should return null, also check the address 
//at one of the written cells to make sure it's correct
TEST(memory_module, one_off) {
    struct RAM* memory = ram_init();

    struct RAM_VALUE i;
    i.value_type = RAM_TYPE_INT;
    i.types.i = 1;

    ram_write_cell_by_name(memory, i, "x");
    ram_write_cell_by_name(memory, i, "y");
    ram_write_cell_by_name(memory, i, "z");
    ram_write_cell_by_name(memory, i, "p");

    ASSERT_EQ(ram_get_addr(memory, "p"), 3);

    ASSERT_EQ(memory->capacity, 4);
    ASSERT_TRUE(ram_read_cell_by_addr(memory, 4) == NULL);

    ram_destroy(memory);
}
