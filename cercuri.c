// Manea Dorin-Mihai, 313CAa
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int x, y, r;
} cerc;

int main(void)
{
	int n;
	scanf("%d", &n);

	//	alocare dinamica de memorie pentru vectorul de cercuri
	cerc *arr = (cerc *)malloc(n * sizeof(cerc));

	//	programare defensiva
	if (!arr) {
		fprintf(stderr, "Nu s-a alocat corect memoria\n");
		return -1;
	}

	//	citire elemente vector de cercuri
	for (int i = 0; i < n; i++)
		scanf("%d%d%d", &arr[i].x, &arr[i].y, &arr[i].r);

	//	verificare intersectie cercuri
	int ct = 0;	//	initializare contor
	for (int i = 0; i < n - 1; i++) {
		int x1 = arr[i].x, y1 = arr[i].y, r1 = arr[i].r;

		for (int j = i + 1; j < n; j++) {
			int x2 = arr[j].x, y2 = arr[j].y, r2 = arr[j].r;

			//	calculul distantei dintre centrele cercurilor de indecsi i si j
			int dist_sq = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);

			//	dist <= r1 + r2 --> cercurile de indecsi i si j se intersecteaza
			if (dist_sq <= (r1 + r2) * (r1 + r2))
				ct++;
		}
	}

	//	afisare contor de perechi de cercuri ce se intersecteaza
	printf("%d\n", ct);

	//	eliberare memorie alocata dinamic
	free(arr);
	return 0;
}
