#ifndef __LIST_H
#define __LIST_H

typedef struct
{
	int item_count;
	int current_max_size;
	char growable;

	void **items;
	char **names;	
} List;

void list_make(List *listo, int size, char growable);
int list_add_item(List *listo, void *item, char *name);
char* list_print_items(List *listo);
void* list_get_name(List *listo, char *name);
void* list_get_index(List *listo, int indx);
void* list_get_item(List *listo, void *item_to_find);
int list_find(List *listo, char *name_to_find);
void list_delete_index(List *listo, int indx);
void list_delete_name(List *listo, char *name);
void list_delete_item(List *listo, void *item);
void list_delete_all(List *listo);
void list_print_list(List *listo);
void list_free(List *listo);

void test_list();
#endif
