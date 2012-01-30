#ifndef ARRAYLIST_H
#define ARRAYLIST_H

typedef struct arraylist_ {
	size_t list_size, elem_size, elem_count, front, back;
	char* array;
} arraylist;


/*	Creates an empty array of size element_size*array_size.
	Fills the given arraylist structure as appropriate.
*/
char init_array(arraylist* al, size_t element_size, size_t array_size);

/* Adds an element to the back of the array, resizing as necessary.
*/
char append_back(arraylist* al, char* elem);

/* Removes an element from the front of the array. 
*/
char remove_front(arraylist* al, char* elem);

/*	Expands the size of the array if there is
	not room for another element.
*/
char expand(arraylist* al);



#endif
