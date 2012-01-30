#include <stdlib.h>
#include <string.h>

#include "arraylist.h"


/*	Creates an empty array of size element_size*array_size.
	Fills the given arraylist structure as appropriate.
*/
char init_array(arraylist* al, size_t element_size, size_t array_size) {
	if(al == NULL || (al->array = malloc(element_size*array_size)) == NULL)
		return 1; //Failure
		
	al->list_size = array_size;
	al->elem_size = element_size;
	al->elem_count = al->front = al->back = 0;
	return 0; //Success
}


/* Adds an element to the back of the array, resizing as necessary.
*/
char append_back(arraylist* al, char* elem) {
	//If empty
	if(al->elem_count == 0) {
		memcpy(&al->array[al->front], elem, al->elem_size);
		al->elem_count++;
		return 0;
	} else { 			
		if(expand(al) != 0) 
			return 1; //Increase the size of this array, if necessary
		al->back += al->elem_size;
		memcpy(&al->array[al->back], elem, al->elem_size);
	}
	
	al->elem_count++;
	return 0;
}

/* Removes an element from the front of the array. 
*/
char remove_front(arraylist* al, char* elem) {
	//If empty
	if(al->elem_count == 0) 
		return 1; //Failure
		
	memcpy(elem, &al->array[al->front], al->elem_size);
	
	al->front += al->elem_size;
	al->elem_count--;	
}

/*	Expands the size of the array if there is
	not room for another element.
*/
char expand(arraylist* al) {
	if( al->elem_count+1 < al->list_size ) 
		return 0; //Nothing to worry about
		
	char* tmp = malloc(al->list_size*al->elem_size*2); //double size of array
	
	if(tmp == NULL)
		return 1; //Failure
	
	memcpy(tmp, &al->array[al->front], al->elem_count * al->elem_size);
	free(al->array);
	al->array = tmp;
	al->list_size = al->list_size*2;
	
	return 0;
}
	


