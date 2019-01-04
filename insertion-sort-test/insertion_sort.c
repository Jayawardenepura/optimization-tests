#include "insertion_sort.h"
void insertion_sort(int *array, int len)
{
	int	i, j;
	int	tmp;

	for (i = 1; i < len; i++) {
		j = i;
		tmp = array[j];
		while (j > 0 && array[j-1] > tmp) {
			array[j] = array[j-1];
			j--;
		}
		array[j] = tmp;
	}
}

