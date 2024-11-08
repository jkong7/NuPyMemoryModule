/*tests.c*/

//
// << DESCRIPTION >>
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

  i.types.i = 5;
  ram_write_cell_by_name(memory, i, "e");
  ASSERT_EQ(memory->num_values, 5);
  ASSERT_EQ(memory->capacity, 8);

  ASSERT_EQ(memory->cells[0].value.types.i, 1);
  ASSERT_EQ(memory->cells[1].value.types.i, 2);
  ASSERT_EQ(memory->cells[2].value.types.i, 3);
  ASSERT_EQ(memory->cells[3].value.types.i, 4);
  ASSERT_EQ(memory->cells[4].value.types.i, 5);

  ram_destroy(memory);
}


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

TEST(memory_module, write_cell_by_name_overwrite) {
    struct RAM* memory = ram_init(); 
    struct RAM_VALUE s1, s2;

    s1.value_type = RAM_TYPE_INT;
    s1.types.i = 42;
    ram_write_cell_by_name(memory, s1, "key");

    s2.value_type = RAM_TYPE_STR;
    s2.types.s = (char*)malloc(strlen("new value") + 1);
    strcpy(s2.types.s, "new value");
    ram_write_cell_by_name(memory, s2, "key");

    ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_STR);
    ASSERT_STREQ(memory->cells[0].value.types.s, "new value");

    free(s2.types.s);
    ram_destroy(memory);
}

TEST(memory_module, write_cell_by_name_string) {
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

TEST(memory_module, edge_case_identifiers) {
    struct RAM* memory = ram_init();

    struct RAM_VALUE i;
    i.value_type = RAM_TYPE_INT;
    i.types.i = 99;

    ASSERT_TRUE(ram_write_cell_by_name(memory, i, ""));
    ASSERT_EQ(memory->cells[0].value.types.i, 99);
    ASSERT_STREQ(memory->cells[0].identifier, "");

    char* long_id = (char*)malloc(1000 * sizeof(char));
    memset(long_id, 'a', 999);
    long_id[999] = '\0';
    i.types.i = 123;
    ASSERT_TRUE(ram_write_cell_by_name(memory, i, long_id));
    ASSERT_EQ(memory->cells[1].value.types.i, 123);
    ASSERT_STREQ(memory->cells[1].identifier, long_id);

    free(long_id);
    ram_destroy(memory);
}

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

    for (int i = 0; i < 10; i++) {
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

TEST(memory_module, concurrent_access_simulation) {
    struct RAM* memory = ram_init();

    struct RAM_VALUE v1, v2;
    v1.value_type = RAM_TYPE_INT;
    v1.types.i = 100;
    ram_write_cell_by_name(memory, v1, "x");

    v2.value_type = RAM_TYPE_REAL;
    v2.types.d = 3.14;
    ram_write_cell_by_name(memory, v2, "y");

    ASSERT_EQ(memory->num_values, 2);

    struct RAM_VALUE* read_x = ram_read_cell_by_name(memory, "x");
    ASSERT_TRUE(read_x != NULL);
    ASSERT_EQ(read_x->types.i, 100);
    ram_free_value(read_x);

    struct RAM_VALUE v3;
    v3.value_type = RAM_TYPE_BOOLEAN;
    v3.types.i = 1;
    ram_write_cell_by_name(memory, v3, "z");
    ASSERT_EQ(memory->num_values, 3);

    struct RAM_VALUE* read_y = ram_read_cell_by_name(memory, "y");
    ASSERT_TRUE(read_y != NULL);
    ASSERT_EQ(read_y->types.d, 3.14);
    ram_free_value(read_y);

    ram_destroy(memory);
}

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


TEST(memory_module, off_by_one_valid_address) {
    struct RAM* memory = ram_init();

    struct RAM_VALUE value1, value2, value3;
    value1.value_type = RAM_TYPE_INT;
    value1.types.i = 10;
    ram_write_cell_by_name(memory, value1, "a");

    value2.value_type = RAM_TYPE_REAL;
    value2.types.d = 20.5;
    ram_write_cell_by_name(memory, value2, "b");

    value3.value_type = RAM_TYPE_STR;
    value3.types.s = strdup("test_string");
    ram_write_cell_by_name(memory, value3, "c");

    struct RAM_VALUE* valid_read = ram_read_cell_by_addr(memory, 1);
    ASSERT_TRUE(valid_read != NULL); 
    ASSERT_EQ(valid_read->value_type, RAM_TYPE_REAL);
    ASSERT_EQ(valid_read->types.d, 20.5);
    ram_free_value(valid_read);

    struct RAM_VALUE* below_valid = ram_read_cell_by_addr(memory, -1); 
    ASSERT_TRUE(below_valid == NULL);

    struct RAM_VALUE* above_valid = ram_read_cell_by_addr(memory, memory->num_values); 
    ASSERT_TRUE(above_valid == NULL);


    struct RAM_VALUE new_value;
    new_value.value_type = RAM_TYPE_INT;
    new_value.types.i = 99;

    bool write_below = ram_write_cell_by_addr(memory, new_value, -1); 
    ASSERT_EQ(write_below, false);

    bool write_above = ram_write_cell_by_addr(memory, new_value, memory->num_values);
    ASSERT_EQ(write_above, false);


    ASSERT_EQ(memory->num_values, 3); 
    ASSERT_EQ(memory->cells[0].value.types.i, 10);
    ASSERT_EQ(memory->cells[1].value.types.d, 20.5);
    ASSERT_STREQ(memory->cells[2].value.types.s, "test_string");

    free(value3.types.s);
    ram_destroy(memory);
}