// Manea Dorin-Mihai, 313CAa
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int n, m;
	scanf("%d%d", &n, &m);

	//	alocare dinamica de memorie pentru matrice + programare defensiva
	int **mat = (int **)calloc(n, sizeof(int *));
	if (!mat) {
		fprintf(stderr, "Nu s-a alocat corect memoria\n");
		return -1;
	}
	for (int i = 0; i < n; i++) {
		mat[i] = (int *)calloc(m, sizeof(int));
		if (!mat[i]) {
			for (int j = 0; j < i; j++)
				free(mat[j]);
			free(mat);
			fprintf(stderr, "Nu s-a alocat corect memoria\n");
			return -1;
		}
	}

	//	citire perechi si incrementare elemente corespunzatoare din matrice
	int k;
	scanf("%d", &k);
	for (int i = 0; i < k; i++) {
		int a, b;
		scanf("%d%d", &a, &b);

		for (int i = 0; i <= a; i++)
			for (int j = 0; j <= b; j++)
				mat[i][j]++;
	}

	//	afisare matrice finala
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			printf("%d ", mat[i][j]);
		printf("\n");
	}

	//	eliberare memorie alocata dinamic
	for (int i = 0; i < n; i++)
		free(mat[i]);
	free(mat);
	return 0;
}
