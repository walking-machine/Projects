#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "winbgi2.h"
//#include "lista.cpp"
#define DT 0.05
#define T1 720


const int mx = 70;
const int my = 14;
int fix[2 * (mx + 1)*(my + 1)];

#include "MesLib.h"
#include "solvers.h"

//struktura reprezentująca macierz rzadką
struct SparseMatrix
{
	int *a;//tablica przechowująca numery wiersza
	int *b;//tablica przechowująca numery kolumny
	double *x;//tablica wartości
	int n;//liczba niezerowych elementów w macierzy sztywności
};

//funkcja dla mnożenia wektora przez macierz rzadką
void SparseMulti(SparseMatrix *M, double* x, double* b, int n) {
	for (int i = 0; i < n; i++) {
		b[i] = 0;
	}
	for (int i = 0; i < M->n; i++) {
		b[M->a[i]] = b[M->a[i]] + M->x[i] * x[M->b[i]];
	}
}

//funkcja tworząca macierz sztywności w postaci macierzy rzadkiej
void SparseSztyw(SparseMatrix *M, double**A, int n) {
	bool reneed;
	//allokacja pamięci
	double* x = (double*)malloc(64*n*sizeof(double));
	int* a = (int*)malloc(64*n * sizeof(int));
	int* b = (int*)malloc(64*n * sizeof(int));
	int k = 0;//liczba elementów w tablicy
	//nakładanie więzów
	for (int i = 0; i < n; i++) {
		if (fix[i] == 1) {
			x[k] = 1;
			a[k] = i;
			b[k] = i;
			k++;
		}
	}

	//iteracja po elementam
	for (int p = 0; p < mx; p++) {
		for (int t = 0; t < my; t++) {
			//iteracja po węzłam elementa
			for (int i = 0; i < 8; i++) {
				int m = DOF(p, t, i);
				if (fix[m] == 1) continue;//nakładanie więzów
				for (int j = 0; j < 8; j++) {
					int v = DOF(p, t, j);
					if (fix[v] == 1) continue;//nakładanie więzów
					reneed = true;
					for (int l = 0; l < k; l++) {//sprawdzenie, czy macierz jeszcze nie zawiera elementu
						if ((a[l] == m)&& (b[l] == v)) {//o takich współrzędnych
							x[l] = x[l] + Md * K[i][j];//jeśli już mamy taki element, to modyfikujemy jego
							reneed = false;//już nie dodajemy nowy element do tablicy
							break;
						}
					}

					if (reneed) {//jeśli takiego elementu jeszcze nie ma w tablicy,
						if (K[i][j] != 0) {//to dodajemy nowy
							x[k] = Md * K[i][j];
							a[k] = m;
							b[k] = v;
							k++;
						}
					}
				}
			}
		}
	}

	//wyrzucamy z tablicy zerowe wartości
	for (int i = 0; i < k; i++) {
		if (x[i] == 0) {
			while (x[k - 1] == 0) k--;
			x[i] = x[k - 1];
			a[i] = a[k - 1];
			b[i] = b[k - 1];
			k = k-1;
		}
	}
	printf("liczba elementow niezerowych w macierzy sztywnosci: %d\n", k);

	//składamy strukturę
	M->a = a;
	M->b = b;
	M->x = x;
	M->n = k;
}

//funkcja, licząca residuum z wykorzystaniem funkcji mnożenia przez macierz rzadką
void SparseResid(int n, SparseMatrix *M, double* b, double* x, double* r) {
	double* temp = new_vector(n);
	SparseMulti(M, x, temp, n);
	for (int i = 0; i < n; i++) r[i] = b[i] - temp[i];
}

//funkcja, licząca współczynnik alfa dla algorytmu gradientów sprzężonych
//z wykorzystaniem funkcji mnożenia przez macierz rzadką
double SparseAlpha(int n, double* r, double* p, SparseMatrix *M) {
	double* mian1 = new_vector(n);
	SparseMulti(M, p, mian1, n);
	double mian2 = mult_vec(mian1, p, n);
	double licz = mult_vec(r, r, n);
	return licz / mian2;
}

//funkcja, rozwiązująca układ równań liniowych z macierzą rzadką za pomocą gradientów sprzężonych
//prawie taka sama jak wersja podstawowa, tylko
//alpha = alpha_grad(n, r, p, A) --> alpha = SparseAlpha(n, r, p, M)
//resid(n, A, b, x, r) --> SparseResid(n, M, b, x, r)
//iloczyn(A, p, add, n) --> SparseMulti(M, p, add, n)
void SparseGradient(int n, SparseMatrix *M, double* b, double* x) {
	double* r = new_vector(n);
	SparseResid(n, M, b, x, r);
	double* p = new_vector(n);
	double* add = new_vector(n);
	double* r1 = new_vector(n);
	double* resid1 = new_vector(n);
	scale(0, -10, 1000, 10);
	copy_vector(r, p, n);
	double alpha = 0;
	double beta = 0;
	for (int m = 0; m < 10000; m++) {
		alpha = SparseAlpha(n, r, p, M);
		add_vector_alpha(x, p, alpha, n);
		SparseMulti(M, p, add, n);
		copy_vector(r, r1, n);
		add_vector_alpha(r, add, -alpha, n);
		beta = beta_grad(n, r1, r);
		copy_vector(r, r1, n);
		add_vector_alpha(r1, p, beta, n);
		copy_vector(r1, p, n);
		SparseResid(n, M, b, x, resid1);
		//printf("%d\t%lf\n", m, norma(resid1, n));
		point(m, norma(resid1, n));
		if (norma(resid1, n) < 0.000001) break;
	}
}

//funkcja mnożąca przez macierz sztywności bez jawnego przechowywania w pamięci
void SMult(double* x, double* b, int n) {
	for (int i = 0; i < n; i++) {
		b[i] = 0;
	}

	for (int p = 0; p < mx; p++) {
		for (int t = 0; t < my; t++) {
			for (int i = 0; i < 8; i++) {
				int m = DOF(p, t, i);
				if (fix[m] == 1) {
					b[m] = x[m];
					continue;
				}
				for (int j = 0; j < 8; j++) {

					int k = DOF(p, t, j);
					if (fix[k] == 1) continue;
					b[m] = b[m] + Md * K[i][j] * x[k];
				}
			}
		}
	}
}

//funkja, układająca zwykłą macierz sztywności
void Sztyw(double **A, int n){

	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			A[i][j] = 0;
		}
	}

	for (int p = 0; p < mx; p++){
		for (int t = 0; t < my; t++){
			for (int i = 0; i < 8; i++){
				for (int j = 0; j < 8; j++){
					int m = DOF(p, t, i);
					int k = DOF(p, t, j);
					A[m][k] = A[m][k] + Md * K[i][j];
				}
			}
		}
	}
}

//definicja więzów i obciążeń
void Fixowanie(double *F, double sila){
	for (int i = 0; i < my + 1; i++){
		fix[P(0, i, 0)] = 1;
		fix[P(0, i, 1)] = 1;
	}
	F[P(mx, 0, 1)] = sila;
}

//nakładanie więzów
void Limit(double **A, double *F, int n) {
	for (int i = 0; i < n; i++) {
		if (fix[i] == 1) {
			for (int j = 0; j < n; j++) {
				A[i][j] = 0;
				A[j][i] = 0;
			}
			A[i][i] = 1;
			F[i] = 0;
		}
	}
}

int main()
{
	int n = 2 * (mx + 1)*(my + 1); // globalna liczba stopni swobody
	double *d = new_vector(n); // Zaalokuj wektor przemieszczen wezlowych
	double *F = new_vector(n); // Zaalokuj wektor sil wezlowych
	double **S0 = new_matrix(n, n);
	double *y = new_vector(n);
	double *b = new_vector(n);
	double *d0 = new_vector(n);
	SparseMatrix *S = (SparseMatrix*)malloc(sizeof(SparseMatrix));
	srand(time(NULL));
	// Przypisz zerowe sily wezlowe i losowe przemieszczenia wezlow
	for (int i = 0; i < n; ++i)
	{
		F[i] = 0;
		d[i] = (double)rand() / (double)RAND_MAX * 10;
		fix[i] = 0;
	}

	copy_vector(d, d0, n);
	Sztyw(S0, n);
	Fixowanie(F, -0.8);
	Limit(S0, F, n);

	int f = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (S0[i][j] != 0) f++;
		}
	}

	printf("\nLiczba niezerowych elementow w podstawowej macierzy: %d\n", f);
	graphics(700, 700);
	//mierzymy czas rozwiązywania dla macierzy w postaci macierzy rzadkiej
	long start = (long)time(NULL);
	SparseSztyw(S, S0, n);
	SparseGradient(n, S, F, d);
	long end = (long)time(NULL);
	printf("\nCzas na rozwiazanie ukladu z macierza w postaci macierzy rzadkiej (w sekundach): %d", end - start);

	printf("\nPress Enter to continue");
	getchar();
	scale(0, 0.5*(my - mx - 3), mx + 3, 0.5*(my + mx + 3));
	title("X", "Y", "MES");
	draw(d, F, fix);
	printf("\nPress Enter to continue");
	getchar();

	//mierzymy czas rozwiązywania dla macierzy w postaci funkcji
	start = (long)time(NULL);
	SGradient(n, SMult, F, d0);
	end = (long)time(NULL);
	printf("\nCzas rozwiązywania ukladu z macierza w postaci funkcji (w sekundach): %d", end - start);

	printf("\nPress Enter to continue");
	getchar();
	scale(0, 0.5*(my - mx - 3), mx + 3, 0.5*(my + mx + 3));
	title("X", "Y", "MES");
	draw(d0, F, fix);
	wait();

	// Zwolnij pamiec
	free_vector(d);
	free_vector(F);
	free_matrix(S0);
	
	// Zakoncz
	return 0;
}