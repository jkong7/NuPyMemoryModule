
/*ram.c*/

//
// In this file, a RAM memory module is implemented that is meant to store nuPython variables and their values. 
// It supports reading and writing of variables and handles memory allocation and deallocation. 
// It uses dynamic memory allocation to handle resizing. 
// The backbone of this memory module is a vector-like data structure where all the read, write, deleting, and resizing take place on. 
//
// Jonathan Kong
//
// Template: Prof. Joe Hummel
// Northwestern University
// CS 211
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // true, false
#include <string.h>
#include <assert.h>

#include "ram.h"


//
// Public functions:
//





//Helper function to copy ram_value, used by the READ operations 
struct RAM_VALUE* copy_ram_value(struct RAM_VALUE val) {
  //First allocate space for the copy 
  struct RAM_VALUE* copy = (struct RAM_VALUE*)malloc(sizeof(struct RAM_VALUE));

  //Now move the fields of value_type and value from original to copy - value_type and actual value (enter conditionals to handle)
  copy->value_type = val.value_type;
  if (val.value_type==RAM_TYPE_INT || val.value_type==RAM_TYPE_PTR || val.value_type == RAM_TYPE_BOOLEAN) {
    copy->types.i = val.types.i;
  } else if (val.value_type==RAM_TYPE_REAL) {
    copy->types.d=val.types.d; 
  } else {
    //Special case of string - need to malloc space for string and then copy original string over 
    char* str = (char*)malloc((strlen(val.types.s)+1)*sizeof(char)); 
    strcpy(str, val.types.s); 
    copy->types.s=str; 
    //copy->types.s=strdup(val.types.s);  achieves ^^ too, will use that everywhere else 
  }
  return copy; 
}






//Used purely by the incremental write operation (write_cell_by_name). This handles the doubling logic 
//Here, the only thing we are messing with is the cells array. First allocate memory for it (2*capacity), then move over all the data from 
//the old cells to the new one (identifier, value_type, and actual value, with strdup used with strings to handle both memory allocation and pointer return)
//Then, FREE the old cells array (loop to free identifier AND string value case), then free the cells. Now, set cells of memory to the new cells and double
//its capacity (num_values stays the same)
void resize_ram(struct RAM* memory) {
  struct RAM_CELL* new_cells = (struct RAM_CELL*)malloc(2*memory->capacity*sizeof(struct RAM_CELL)); 
  for (int i=0; i<memory->capacity; i++) {
    new_cells[i].identifier = strdup(memory->cells[i].identifier); 
    new_cells[i].value.value_type = memory->cells[i].value.value_type;
    if (memory->cells[i].value.value_type == RAM_TYPE_INT || memory->cells[i].value.value_type == RAM_TYPE_BOOLEAN || memory->cells[i].value.value_type == RAM_TYPE_PTR) {
      new_cells[i].value.types.i = memory->cells[i].value.types.i;
    } else if (memory->cells[i].value.value_type == RAM_TYPE_REAL) {
      new_cells[i].value.types.d = memory->cells[i].value.types.d;
    } else if (memory->cells[i].value.value_type == RAM_TYPE_STR) {
      new_cells[i].value.types.s = strdup(memory->cells[i].value.types.s);
    }  
  }
  for (int i=0; i<memory->capacity;i++) {
    free(memory -> cells[i].identifier); 
    if (memory->cells[i].value.value_type==RAM_TYPE_STR) {
      free(memory->cells[i].value.types.s); 
    } 
  }
  free(memory->cells);
  memory->cells = new_cells;
  memory->capacity *= 2;
}






//
// ram_init
//
// Returns a pointer to a dynamically-allocated memory
// for storing nuPython variables and their values. All
// memory cells are initialized to the value None.
//
struct RAM* ram_init(void)
{
  struct RAM* memory = (struct RAM*)malloc(sizeof(struct RAM)); //Allocate memeory for RAM* and return pointer to it 

  //Set fields of memory (initialization, so num_values: 0, capacity: 4, and cells: pointer to malloced array of cells)
  memory -> num_values = 0; 
  memory -> capacity = 4; 
  memory -> cells = (struct RAM_CELL*)malloc(memory->capacity*sizeof(struct RAM_CELL)); 
  return memory; 
}





//
// ram_destroy
//
// Frees the dynamically-allocated memory associated with
// the given memory. After the call returns, you cannot
// use the memory.
//
void ram_destroy(struct RAM* memory)
{
  //free deallocates the memory block pointed by the given pointer, but does NOT recursively free memory that the contents may point to
  //Therefore, all of these need to be explicitly freed since all nested inside larger vector DS: identifier, string if ram_type_str, cells, and finally whole block
  for (int i=0; i<memory->num_values;i++) {
    free(memory -> cells[i].identifier); 
    if (memory->cells[i].value.value_type==RAM_TYPE_STR) {
      free(memory->cells[i].value.types.s); 
    } 
  }
  free(memory->cells); 
  free(memory); 
}





//
// ram_get_addr
// 
// If the given identifier (e.g. "x") has been written to 
// memory, returns the address of this value --- an integer
// in the range 0..N-1 where N is the number of values currently 
// stored in memory. Returns -1 if no such identifier exists 
// in memory. 
// 
// NOTE: a variable has to be written to memory before you can
// get its address. Once a variable is written to memory, its
// address never changes.
//
int ram_get_addr(struct RAM* memory, char* identifier)
{
  for (int i=0; i<memory->num_values;i++) {
    //Use strcmp to compare strings, simple loop to find index, -1 if not found
    if (strcmp(memory->cells[i].identifier,identifier)==0) { 
      return i; 
    }
  }
  return -1;
}





//
// ram_read_cell_by_addr
//
// Given a memory address (an integer in the range 0..N-1), 
// returns a COPY of the value contained in that memory cell.
// Returns NULL if the address is not valid.
//
// NOTE: this function allocates memory for the value that
// is returned. The caller takes ownership of the copy and 
// must eventually free this memory via ram_free_value().
//
// NOTE: a variable has to be written to memory before its
// address becomes valid. Once a variable is written to memory,
// its address never changes.
//
struct RAM_VALUE* ram_read_cell_by_addr(struct RAM* memory, int address)
{
  //Handle out of bounds address
  if (address >= memory->num_values || address < 0) {
    return NULL;
  }

  //Find cell using input address index 
  struct RAM_VALUE cell = memory->cells[address].value;

  //Call helper function to copy the cell and return 
  return copy_ram_value(cell); 
}




// 
// ram_read_cell_by_name
//
// If the given name (e.g. "x") has been written to 
// memory, returns a COPY of the value contained in memory.
// Returns NULL if no such name exists in memory.
//
// NOTE: this function allocates memory for the value that
// is returned. The caller takes ownership of the copy and 
// must eventually free this memory via ram_free_value().
//
struct RAM_VALUE* ram_read_cell_by_name(struct RAM* memory, char* name)
{
  //Use ram_get_addr to return address given the name identifier 
  int address = ram_get_addr(memory, name); 
  //Use ram_read_cell_by_addr to handle making copy now that we have name identifier (also handle edge case name not contained)
  return (address==-1) ? NULL : ram_read_cell_by_addr(memory, address); 
}




//
// ram_free_value
//
// Frees the memory value returned by ram_read_cell_by_name and
// ram_read_cell_by_addr.
//
void ram_free_value(struct RAM_VALUE* value)
{
  //Idenfitier and value: free the dynamic memory (strings), which are ram_type_str case for value along with the identifier 
  if (value == NULL) return;
  //String case: have to explictiy free what pointer is pointing to (char*)
  if (value->value_type==RAM_TYPE_STR) {
    free(value->types.s); 
  }
  free(value);
}





//
// ram_write_cell_by_addr
//
// Writes the given value to the memory cell at the given 
// address. If a value already exists at this address, that
// value is overwritten by this new value. Returns true if 
// the value was successfully written, false if not (which 
// implies the memory address is invalid).
// 
// NOTE: if the value being written is a string, it will
// be duplicated and stored.
// 
// NOTE: a variable has to be written to memory before its
// address becomes valid. Once a variable is written to memory,
// its address never changes.
//
bool ram_write_cell_by_addr(struct RAM* memory, struct RAM_VALUE value, int address)
{
  //Handle out of bounds cases
  if (address>=memory->num_values || address < 0) {
    return false; 
  }

  //This function is purely for OVERWRITING - if we got here, that means we are going to overwrite current cell 

  //String case - need to explicitly deallocate memory first 
  if (memory->cells[address].value.value_type==RAM_TYPE_STR) {
    free(memory->cells[address].value.types.s); 
  } 
  //Overwrite: 
  memory -> cells[address].value.value_type = value.value_type;
  if (value.value_type == RAM_TYPE_INT || value.value_type == RAM_TYPE_BOOLEAN || value.value_type == RAM_TYPE_PTR) {
    memory -> cells[address].value.types.i = value.types.i;
  } else if (value.value_type == RAM_TYPE_REAL) {
    memory -> cells[address].value.types.d = value.types.d;
  } else if (value.value_type == RAM_TYPE_STR) {
    memory -> cells[address].value.types.s = strdup(value.types.s); //strdup mallocs memory and returns pointer, conveniant! 
  }  
  return true; 
}





//
// ram_write_cell_by_name
//
// Writes the given value to a memory cell named by the given
// name. If a memory cell already exists with this name, the
// existing value is overwritten by the given value. Returns
// true since this operation always succeeds.
// 
// NOTE: if the value being written is a string, it will
// be duplicated and stored.
// 
// NOTE: a variable has to be written to memory before its
// address becomes valid. Once a variable is written to memory,
// its address never changes.
//
bool ram_write_cell_by_name(struct RAM* memory, struct RAM_VALUE value, char* name)
{
  //First get the index using ram_get_addr
  int address = ram_get_addr(memory, name);

  //If cell with name already exists in memory, nothing fancy, just overwrite using write_cell_by_addr! 
  if (address!=-1) {
    ram_write_cell_by_addr(memory, value, address); 
    return true; 
  } 

  //Otherwise: We have to create a new cell at index num-values (0 indexed so num-values represents "next" cell)

  //Here is where we have to first check if we need to double 
  if (memory->capacity==memory->num_values) {
    resize_ram(memory); 
  }

  //Put the fields in, for identifier - strdup to handle memory allocation and pointer return at once, for actual value - enter conditionals 
  memory -> cells[memory->num_values].identifier = strdup(name); 
  memory -> cells[memory->num_values].value.value_type = value.value_type;
  if (value.value_type == RAM_TYPE_INT || value.value_type == RAM_TYPE_BOOLEAN || value.value_type == RAM_TYPE_PTR) {
    memory -> cells[memory->num_values].value.types.i = value.types.i;
  } else if (value.value_type == RAM_TYPE_REAL) {
    memory -> cells[memory->num_values].value.types.d = value.types.d;
  } else if (value.value_type == RAM_TYPE_STR) {
    memory -> cells[memory->num_values].value.types.s = strdup(value.types.s);
  } 

  //num_values incremented as a new value was written
  memory->num_values+=1; 
  
  return true; 
}





//
// ram_print
//
// Prints the contents of memory to the console.
//
void ram_print(struct RAM* memory)
{
  printf("**MEMORY PRINT**\n");

  printf("Capacity: %d\n", memory->capacity);
  printf("Num values: %d\n", memory->num_values);
  printf("Contents:\n");

  //Use switch to handle conditional printing logic 
  for (int i = 0; i < memory->num_values; i++)
  {
      printf(" %d: %s, ", i, memory->cells[i].identifier);
      switch (memory->cells[i].value.value_type) {
        case RAM_TYPE_INT: 
          printf("int, %d", memory->cells[i].value.types.i);
          break;
        case RAM_TYPE_REAL:
          printf("real, %lf", memory->cells[i].value.types.d);
          break;
        case RAM_TYPE_STR:
          printf("str, '%s'", memory->cells[i].value.types.s);
          break; 
        case RAM_TYPE_PTR: 
          printf("ptr, %d", memory->cells[i].value.types.i);
          break; 
        case RAM_TYPE_BOOLEAN: 
          if (memory->cells[i].value.types.i == 0) {
            printf("boolean, False");
          } else {
            printf("boolean, True");
          }
          break; 
        default: 
          printf("none, None");
          break; 
      }

      printf("\n");
  }

  printf("**END PRINT**\n");
}
