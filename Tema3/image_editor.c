//Nastase Maria
//311 CA
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#define NMAX 1001

int verif(char *comanda)
{
	//aflare comanda
	int i = -1;
	if (strcmp(comanda, "LOAD") == 0)
		i = 1;
	else if (strcmp(comanda, "SELECT") == 0)
		i = 2;
	else if (strcmp(comanda, "ROTATE") == 0)
		i = 3;
	else if (strcmp(comanda, "CROP") == 0)
		i = 4;
	else if (strcmp(comanda, "GRAYSCALE") == 0)
		i = 5;
	else if (strcmp(comanda, "SEPIA") == 0)
		i = 6;
	else if (strcmp(comanda, "SAVE") == 0)
		i = 7;
	else if (strcmp(comanda, "EXIT") == 0)
		i = 8;
	return i;
}

int rotunjire(double nr)
{
	//aproximare nr la cel mai apropiat int
	nr = nr * 10;
	int num = (int)nr;
	if (num % 10 >= 5)
		return num / 10 + 1;
	else
		return num / 10;
}

int nr_cuvinte(char *sir, int *index)
{
	//aflare nr cuvinte in sir
	int nr = 1;
	for (size_t i = 0; i < strlen(sir); i++) {
		if (i < strlen(sir) - 2) {
			if (sir[i] == ' ' && sir[i + 1] != ' ') {
				nr++;
				if (nr == 2)
					*index = i + 1; //aflare index cuvant 2
			}
		}
	}
	return nr;
}

int aflare_coord(char *sir, int *cx1, int *cx2, int *cy1, int *cy2)
{
	//extragere coordonate date ca argumente
	int da = 1, nr = 0;
	char *p = strtok(sir, "\n ");
	while (p && da == 1) {
		//verificare cuvant = numar
		for (size_t i = 0; i < strlen(p) && da == 1; i++) {
			if (p[i] >= 'a' && p[i] <= 'z')
				da = 0;
			if (p[i] >= 'A' && p[i] <= 'Z')
				da = 0;
		}
		if (da != 0) {
			if (nr == 0)
				*cx1 = atoi(p);
			else if (nr == 1)
				*cy1 = atoi(p);
			else if (nr == 2)
				*cx2 = atoi(p);
			else
				*cy2 = atoi(p);
		}
		nr++;
		p = strtok(NULL, "\n ");
	}
	return da;
}

unsigned char **alocare_matrice(int n, int m)
{
	// alocare vector de linii
	unsigned char **a = (unsigned char **)malloc(n * sizeof(unsigned char *));
	if (!a) {
		fprintf(stderr, "malloc() failed\n");
		return NULL;
	}

	// alocare linii
	for (int i = 0; i < n; ++i) {
		a[i] = (unsigned char *)malloc((m + 1) * sizeof(unsigned char));
		if (!a[i]) {
			fprintf(stderr, "malloc() failed\n");

			// elibereaza memoria alocata pentru liniile anterioare
			while (--i >= 0)
				free(a[i]);
			free(a);
			return NULL;
		}
	}
	return a;
}

void free_matrice(int n, unsigned char **a)
{
	//dealocare linii
	for (int i = 0; i < n; ++i)
		free(a[i]);
	free(a);
}

void schimbare_char(unsigned char *a, unsigned char *b)
{
	//interschimbare a si b de tip const char
	unsigned char aux;
	aux = *a;
	*a = *b;
	*b = aux;
}

void schimbare_int(int *a, int *b)
{
	//interschimbare a si b de tip int
	int aux;
	aux = *a;
	*a = *b;
	*b = aux;
}

void citire_matrice_bin(int n, int m, unsigned char **a, FILE *img)
{
	//citire matrice din fisier binar
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; j++)
			fread(&a[i][j], sizeof(unsigned char), 1, img);
}

void citire_matrice_text(int n, int m, unsigned char **a, FILE *img)
{
	//citire matrice din fisier text
	unsigned char x;
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; j++) {
			fscanf(img, "%hhd", &x);
			a[i][j] = x;
		}
	}
}

void load(int *n, int *m, int *tip, int *culoare, int *x1, int *x2,
		  int *y1, int *y2, int *ok, int nr, unsigned char ***a, char *sir)
{
	//comanda load
	int binar = 0, var = 4, i = 1, x;
	if (nr > 2 || nr == 1) {
		printf("Invalid command\n");
	} else {
		char numefis[NMAX];
		strcpy(numefis, sir);
		//deschidere fisier in format text
		FILE *img = fopen(numefis, "rt");
		if (!img) {
			printf("Failed to load %s\n", numefis);
			*ok = 0; //nu e incarcat un fisier
		} else {
			int nfost = *n, mfost = *m;
			printf("Loaded %s\n", numefis);
			binar = 0;
			fseek(img, 1, SEEK_SET);
			while (i <= var) {
				//citire tip, nr coloane, nr linii, culoare max
				if (fscanf(img, "%d", &x) == 1) {
					if (i == 1)
						*tip = x;
					else if (i == 2)
						*m = x;
					else if (i == 3)
						*n = x;
					else if (i == 4)
						*culoare = x;
					++i;
				} else {
					fseek(img, 1, SEEK_CUR);
				}
			}
			if (*tip == 3 || *tip == 6)
				*m = (*m) * 3;
			*x1 = 0;
			*x2 = *m;
			*y1 = 0;
			*y2 = *n;
			int nr_bytes = ftell(img);
			if (*tip == 5 || *tip == 6) {
				binar = 1;
				fclose(img);
				//deschidere fisier in format binar
				FILE *img = fopen(numefis, "rb");
				if (!img)
					fprintf(stderr, "Cannot open %s file\n", numefis);
			}
			if (*ok == 1) {
				if (*n != nfost || *m != mfost) {
					free_matrice(nfost, *a);
					*a = alocare_matrice(*n, *m);
				}
			} else {
				*a = alocare_matrice(*n, *m);
			}
			if (!(*a))
				fprintf(stderr, "Failed\n");
			fseek(img, nr_bytes + 1, SEEK_SET);
			if (binar == 0)
				citire_matrice_text(*n, *m, *a, img);
			else
				citire_matrice_bin(*n, *m, *a, img);
			fclose(img);
			*ok = 1;
		}
	}
}

void selectare(int n, int m, int tip, int *x1, int *x2, int *y1, int *y2,
			   int ok, int nr_cuv, char *sir)
{
	if (ok == 0) {
		printf("No image loaded\n");
	} else if (nr_cuv == 1 || nr_cuv == 3 || nr_cuv == 4) {
		printf("Invalid command\n");
	} else if (nr_cuv == 5) {
		int nr = 1, cx1, cx2, cy1, cy2;
		if (aflare_coord(sir, &cx1, &cx2, &cy1, &cy2) == 0) {
			printf("Invalid command\n");
		} else {
			if (tip == 3 || tip == 6) {
				//coordonate pt imagini color
				cx1 = cx1 * 3;
				cx2 = cx2 * 3;
				nr = 3;
			}
			if (cy1 > cy2)
				schimbare_int(&cy1, &cy2);
			if (cx1 > cx2)
				schimbare_int(&cx1, &cx2);
			if (cx1 >= 0 && cx2 <= m && cy1 >= 0 && cy2 <= n &&
				cx1 != cx2 && cy2 != cy1) {
				*x1 = cx1;
				*x2 = cx2;
				*y1 = cy1;
				*y2 = cy2;
				printf("Selected %d %d %d %d\n", *x1 / nr, *y1, *x2 / nr, *y2);
			} else {
				printf("Invalid set of coordinates\n");
			}
		}
	} else if (nr_cuv == 2) {
		char *p = strtok(sir, "\n ");
		if (strcmp(p, "ALL") == 0) {
			printf("Selected ALL\n");
			*x1 = 0;
			*x2 = m;
			*y1 = 0;
			*y2 = n;
		} else {
			printf("Invalid command\n");
		}
	}
}

void matrice_selectata(int x1, int x2, int y1, int y2, unsigned char **a,
					   unsigned char **b)
{
	//creare imagine cu pixelii selectati
	int col = 0, lin = 0;
	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			b[lin][col] = a[i][j];
			col++;
		}
		lin++;
		col = 0;
	}
}

void rotatie_dr(int n, int m, int tip, int unghi,
				unsigned char **a, unsigned char **b)
{
	//rotatie a unui dreptunghi
	int lin, col, nr = 3, numlin, numcol;
	lin = 0;
	col = 0;
	if (tip != 3 && tip != 6)
		nr = 1;
	numlin = m / nr;
	numcol = n * nr;
	if (unghi == 90 || unghi == 270) {
		//rotire la 90 de grade
		for (int i = 0; i < numcol; i = i + nr) {
			for (int j = numlin - 1; j >= 0; j--) {
				if (nr == 3) {
					a[lin][col] = b[j][i]; //rotatie rosu
					a[lin][col + 1] = b[j][i + 1]; //rotatie albastru
					a[lin][col + 2] = b[j][i + 2]; //rotatie verde
					col += 3;
				} else {
					a[lin][col] = b[j][i]; //rotatie gri
					col++;
				}
			}
			lin++;
			col = 0;
		}
	}
	if (unghi > 90) {
		numcol = m;
		int ok = 0;
		if (n % 2 == 1)
			ok = 1;
		for (int i = 0; i < n / 2; i++) {
			if (i == n / 2 - 1 && ok == 1)
				numcol = m / 2;
			for (int j = 0; j < numcol; j = j + nr) {
				if (nr == 1) {
					//rotatie 180 sau 270 gri
					schimbare_char(&a[i][j], &a[n - i - 1][m - j - 1]);
				} else {
					//rotatie 180 sau 270 rosu, albastru, verde
					schimbare_char(&a[i][j], &a[n - i - 1][m - j - 3]);
					schimbare_char(&a[i][j + 1], &a[n - i - 1][m - j - 2]);
					schimbare_char(&a[i][j + 2], &a[n - i - 1][m - j - 1]);
				}
			}
		}
	}
}

void rotatie_patrat(int x1, int x2, int y1, int y2, int tip, unsigned char **a)
{
	//rotatie a patratului
	int nr = 1, k, numar = 0, lin = 0, col = 0;
	if (tip == 3 || tip == 6)
		nr = 3;
	if (nr == 1) {
		//rotatie gri
		for (int i = y1; i < y2; i++) {
			//construire transpusa pixelilor selectati
			lin = i;
			col = x1 + numar;
			for (int j = x1 + numar; j < x2; j++) {
				schimbare_char(&a[i][j], &a[lin][col]);
				lin++;
			}
			numar++;
		}
		for (int i = y1; i < y2; i++) {
			//rotatie 90 de grade
			k = x2 - 1;
			for (int j = x1; j < k; j++) {
				schimbare_char(&a[i][j], &a[i][k]);
				k--;
			}
		}
	} else {
		//rotatie color
		for (int i = y1; i < y2; i++) {
			col = x1 + numar * 3;
			lin = i;
			for (int j = x1 + numar * 3; j < x2; j += 3) {
				//construire transpusa pixelilor selectati
				schimbare_char(&a[i][j], &a[lin][col]);
				schimbare_char(&a[i][j + 1], &a[lin][col + 1]);
				schimbare_char(&a[i][j + 2], &a[lin][col + 2]);
				lin++;
			}
			numar++;
		}
		for (int i = y1; i < y2; i++) {
			//rotatie 90 de grade
			k = x2 - 3;
			for (int j = x1; j < k; j = j + 3) {
				schimbare_char(&a[i][j], &a[i][k]);
				schimbare_char(&a[i][j + 1], &a[i][k + 1]);
				schimbare_char(&a[i][j + 2], &a[i][k + 2]);
				k = k - 3;
			}
		}
	}
}

void aflare_unghi(int *unghi)
{
	//prelucrare val unghi ([0, 360) grade)
	if (*unghi < 0) {
		while (*unghi < 0)
			*unghi = *unghi + 360;
	} else if (*unghi >= 360) {
		while (*unghi >= 360)
			*unghi = *unghi - 360;
	}
}

void rotate(int *n, int *m, int tip, int x1, int *x2, int y1, int *y2,
			int ok, int nr, unsigned char ***a, char *valrot)
{
	//comanda rotate
	if (ok == 0) {
		printf("No image loaded\n");
	} else if (nr > 2 || nr == 1) {
		printf("Invalid set of coordinates\n");
	} else {
		int lat, patrat = 1, nb, i = 1, unghi1, rot = 1, unghi;
		unsigned char **b;
		if (valrot[0] == '-') //valoare negativa
			unghi = (-1) * atoi(valrot + 1);
		else
			unghi = atoi(valrot);
		if (tip != 3 && tip != 6) {
			lat = *x2 - x1; //gri
		} else {
			lat = (*x2 - x1) / 3; //color
			i = 3;
		}
		if (unghi % 90 != 0) {
			printf("Unsupported rotation angle\n");
		} else {
			unghi1 = unghi; //copie unghi
			aflare_unghi(&unghi1);
			if (lat != *y2 - y1)
				patrat = 0;
			if (*n == *y2 - y1 && *m == *x2 - x1 && patrat == 0) {
				nb = *n;
				b = alocare_matrice(*n, *m); //copie matrice initiala
				matrice_selectata(x1, *x2, y1, *y2, *a, b);
				if ((unghi1 / 90) % 2 == 1) {
					free_matrice(*n, *a);
					*m = *m / i;
					*n = *n * i;
					schimbare_int(n, m);
					*a = alocare_matrice(*n, *m);
				}
			} else if (*n != *y2 - y1 && *m != *x2 - x1 && patrat == 0) {
				rot = 0; //nu se poate roti
				printf("The selection must be square\n");
			}
			if (rot == 1) {
				if (patrat == 1) {
					while (unghi1 > 0) {
						rotatie_patrat(x1, *x2, y1, *y2, tip, *a);
						unghi1 = unghi1 - 90;
					}
					printf("Rotated %d\n", unghi);
				} else {
					rotatie_dr(*n, *m, tip, unghi1, *a, b);
					free_matrice(nb, b);
					*y2 = *n;
					*x2 = *m;
					printf("Rotated %d\n", unghi);
				}
			}
		}
	}
}

void crop(int *n, int *m, int *x1, int *x2, int *y1, int *y2,
		  int ok, int nr_cuv, unsigned char ***a)
{
	//comanda crop
	unsigned char **b;
	if (ok == 0) {
		printf("No image loaded\n");
	} else if (nr_cuv > 1) {
		printf("Invalid command\n");
	} else {
		if (*n != *y2 - *y1 || *m != *x2 - *x1) {
			b = alocare_matrice(*y2 - *y1, *x2 - *x1);
			//creare copie matrice selectata
			matrice_selectata(*x1, *x2, *y1, *y2, *a, b);
			free_matrice(*n, *a);
			*a = alocare_matrice(*y2 - *y1, *x2 - *x1);
			*n = *y2 - *y1;
			*m = *x2 - *x1;
			*x1 = 0;
			*x2 = *m;
			*y1 = 0;
			*y2 = *n;
			for (int i = 0; i < *n; i++)
				for (int j = 0; j < *m; j++)
					(*a)[i][j] = b[i][j];
			//*a = b;
			free_matrice(*n, b);
		}
		printf("Image cropped\n");
	}
}

void grayscale(int x1, int x2, int y1, int y2, int tip, int ok,
			   int nr, unsigned char **a)
{
	//aplicare filtru grayscale
	if (ok == 0) {
		printf("No image loaded\n");
	} else if (nr > 1) {
		printf("Invalid command\n");
	} else if (tip == 3 || tip == 6) {
		printf("Grayscale filter applied\n");
		for (int i = y1; i < y2; i++) {
			for (int j = x1; j < x2; j += 3) {
				double culoare_noua = (a[i][j] + a[i][j + 1] + a[i][j + 2]) / 3;
				culoare_noua = rotunjire(culoare_noua);
				a[i][j] = (int)culoare_noua; //rosu
				a[i][j + 1] = (int)culoare_noua; //verde
				a[i][j + 2] = (int)culoare_noua; //albastru
			}
		}
	} else {
		printf("Grayscale filter not available\n");
	}
}

void sepia(int x1, int x2, int y1, int y2, int culoare, int tip, int ok,
		   int nr, unsigned char **a)
{
	//aplicare filtru sepia
	if (ok == 0) {
		printf("No image loaded\n");
	} else if (tip != 3 && tip != 6) {
		printf("Sepia filter not available\n");
	} else if (nr > 1) {
		printf("Invalid set of coordinates\n");
	} else {
		printf("Sepia filter applied\n");
		double r, g, b;
		for (int i = y1; i < y2; i++) {
			for (int j = x1; j < x2; j += 3) {
				r = 0.393 * a[i][j] + 0.769 * a[i][j + 1] + 0.189 * a[i][j + 2];
				g = 0.349 * a[i][j] + 0.686 * a[i][j + 1] + 0.168 * a[i][j + 2];
				b = 0.272 * a[i][j] + 0.534 * a[i][j + 1] + 0.131 * a[i][j + 2];
				if (r > culoare)
					r = culoare;
				if (g > culoare)
					g = culoare;
				if (b > culoare)
					b = culoare;
				a[i][j] = rotunjire(r); //rosu
				a[i][j + 1] = rotunjire(g); //verde
				a[i][j + 2] = rotunjire(b); //albastru
			}
		}
	}
}

void save(int n, int m, int tip, int ok, int culoare, int nr_cuv,
		  unsigned char **a, char *save)
{
	//comanda save
	int nr = 1, bin = 0, numcol = m;
	char numefis[NMAX] = "", com[NMAX] = "", str[] = "ascii";
	if (ok == 0) {
		printf("No image loaded\n");
	} else if (nr_cuv > 3 || nr_cuv == 1) {
		printf("Invalid command\n");
	} else {
		char *p = strtok(save, "\n ");
		while (p) {
			if (nr == 1)
				strcpy(numefis, p); //extragere nume fisier
			else if (nr == 2)
				strcpy(com, p);
			nr++;
			p = strtok(NULL, "\n ");
		}
		if (tip == 3 || tip == 6)
			numcol = m / 3;
		if (nr_cuv == 2) {
			bin = 1;
		} else {
			//verificare argument2 = "ascii"
			for (size_t i = 0; i < strlen(str) && bin == 0; i++)
				if (com[i] != str[i])
					bin = -1;
		}
		if (bin == -1) {
			printf("Invalid command\n");
		} else if (bin == 0) {
			//salvare text
			FILE *filesave = fopen(numefis, "wt");
			if (tip > 3)
				tip -= 3;
			if (!filesave) {
				fprintf(stderr, "Cannot open %s file", numefis);
			} else {
				printf("Saved %s\n", numefis);
				fprintf(filesave, "P%d\n", tip);
				fprintf(filesave, "%d %d\n", numcol, n);
				fprintf(filesave, "%d\n", culoare);
				for (int i = 0; i < n; i++) {
					for (int j = 0; j < m; j++)
						fprintf(filesave, "%d ", a[i][j]);
					fprintf(filesave, "\n");
				}
				fclose(filesave);
			}
		} else {
			//salvare binara
			if (tip <= 3)
				tip += 3;
			FILE *filesave = fopen(numefis, "wb");
			if (!filesave) {
				fprintf(stderr, "Cannot open %s file\n", numefis);
			} else {
				printf("Saved %s\n", numefis);
				fprintf(filesave, "P%d\n", tip);
				fprintf(filesave, "%d %d\n", numcol, n);
				fprintf(filesave, "%d\n", culoare);
				for (int i = 0; i < n; i++) {
					for (int j = 0; j < m; j++)
						fwrite(&a[i][j], sizeof(unsigned char), 1, filesave);
					}
				fclose(filesave);
			}
		}
	}
}

int main(void)
{
	int n, m, tip, x1, x2, y1, y2, culoare = 0, ok = 0,
		exit = 0, com, nr_cuv = 0, index;
	unsigned char **a;
	while (exit != 1) {
		char sir[NMAX] = "", copie[NMAX] = "";
		fgets(sir, NMAX, stdin);
		nr_cuv = nr_cuvinte(sir, &index);
		if (nr_cuv > 1) {
			strcpy(copie, sir + index);
			copie[strlen(copie) - 1] = '\0';
		}
		char *p = strtok(sir, "\n ");
		com = verif(p);
		switch (com) {
			case 1: {
				// cazul LOAD
				load(&n, &m, &tip, &culoare, &x1, &x2, &y1, &y2,
					 &ok, nr_cuv, &a, copie);
			}
			break;
			case 2: {
				// cazul SELECT
				selectare(n, m, tip, &x1, &x2, &y1, &y2, ok, nr_cuv, copie);
			}
				break;
			case 3: {
				// cazul ROTATE
				rotate(&n, &m, tip, x1, &x2, y1, &y2, ok, nr_cuv, &a, copie);
			}
				break;
			case 4: {
				//cazul CROP
				crop(&n, &m, &x1, &x2, &y1, &y2, ok, nr_cuv, &a);
			}
				break;
			case 5: {
				//cazul GRAYSCALE
				grayscale(x1, x2, y1, y2, tip, ok, nr_cuv, a);
			}
				break;
			case 6: {
				//cazul SEPIA
				sepia(x1, x2, y1, y2, culoare, tip, ok, nr_cuv, a);
			}
				break;
			case 7: {
				//cazul SAVE
				save(n, m, tip, ok, culoare, nr_cuv, a, copie);
			}
				break;
			case 8: {
				//cazul EXIT
				if (nr_cuv > 1) {
					printf("Invalid command\n");
				} else {
					if (ok == 0)
						printf("No image loaded\n");
					exit = 1;
				}
			}
				break;
			default: {
				//comanda gresita
				printf("Invalid command\n");
			}
				break;
		}
	}
	free_matrice(n, a);
	return 0;
}
