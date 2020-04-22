#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "winbgi2.h"
#define A 5 //wymiar 1 komórki;
#define B 10 // 1/B = stężenie żywych komórek w stanie początkowym (w tym przypadku każda 10ta komórka będzie żywą);

//rysuje pojedynczą komórkę;
void draw_cell(int x, int y){
	int i = 0;
	for (i = 0; i < A; i++) {
		line(x * A, y * A + i, x * A + A, y * A + i);
	}
}

//liczy żywych sąsiadów pojedynczej komórki
int count_ln(short **cells, int num1, int num2, int cnum1, int cnum2) {
	//liczba sąsiadów
	int count = 0;
	int i, j;
	//sprawdzamy wszystkich sąsiadów
	for (i = -1; i <=1; i++) {
		//jeżeli sąsiad jest poza granicami tablicy, to uważa sie za martwego, czyli nic nie dodajemy do zmiennej count
		if ((num1 + i >= 0)&&(num1 + i < cnum1)) {
			for (j = -1; j <= 1; j++) {
				if ((j!=0)||(i!=0)) {//pomijamy komórkę, sąsiadów której liczymy
					if ((num2 + j >= 0) && (num2 + j < cnum2)) {
						if (cells[num1 + i][num2 + j] > 0) {
							count++;
						}
					}
				}
			}
		}
	}

	return count;

}

//zmienia tablicę komórek
void update(short **cells, int cnum1, int cnum2) {
	short **cells2;//tablica, przechowująca stan początkowy (tablica główna będzie ulegała zmianie)
	cells2 = (short **)malloc(cnum1 * sizeof(short *));
	for (int k = 0; k < cnum1; ++k)
		cells2[k] = (short *)malloc(cnum2 * sizeof(short));

	int i, j;
	for (i = 0; i < cnum1; i ++) {
		for (j = 0; j < cnum2; j++) {
			cells2[i][j] = cells[i][j];
		}
	}
	//zmieniamy tablicę według reguł Conway'a
	for (i = 0; i < cnum1; i++) {
		for (j = 0; j < cnum2; j++) {
			int count = count_ln(cells2, i, j, cnum1, cnum2);
			if (cells2[i][j] > 0) {

				if ((count > 3)||(count < 2)) {//żywa komórka może umrzeć z powodu samotności (mniej niż 2 żywych sąsiada) lub zatłoczenia (więcej niż 3 żywych sąsiada)
					cells[i][j] = 0;
				}
			}
			else {
				if (count == 3) {//martwa komórka, która ma dokładnie 3 żywych sąsiadów, staje się żywa
					cells[i][j] = 1;
				}
			}

			
		}
	}
	//usuwamy drugą tablicę
	for (int i = 0; i < cnum1; ++i)
		free(cells2[i]);
	free(cells2);
}

//wytwarza tablicę o przypadkowych wartościach (0 albo 1, martwa albo żywa komórka)
void init_cells(short **cells, int cnum1, int cnum2) {
	int i, j;
	for (i = 0; i < cnum1; i++) {
		for (j = 0; j < cnum2; j++) {
			cells[i][j] = rand() % B == 0 ? 1 : 0;
		}
	}
}


//rysuje tablicę komórek
void draw(short **cells, int cnum1, int cnum2) {
	clear();//usuwa wszystko narysowane w poprzedniej jednostce czasu
	int i, j;
	for (i = 0; i < cnum1; i++) {
		for (j = 0; j < cnum2; j++) {
			if (cells[i][j] == 1) {
				draw_cell(i, j);
			}
		}


	}
}


int main(void) {

srand(time(NULL));
	//zmienne przechowujące wymiary okna
	int N = 40;
	int N2 = 40;
	printf("Define size (int)<space>(int) not bigger than 80by80\n");
	scanf("%d %d", &N, &N2);
	//ta pętla gwarantuje to, ży zostaną wprowadzone właściwe wartości
	while ((N>80)||(N<2)|| (N2>80) || (N2<2)) {
		printf("Niewlasciwe wartosci. Wpisz jeszcze raz\n");
		scanf("%d %d\n", &N, &N2);
	}
	//główna tablica, zawierająca dane o komórkach
	short **cells;
	cells = (short **)malloc(N * sizeof(short *));
	for (int i = 0; i < N; ++i)
		cells[i] = (short *)malloc(N2 * sizeof(short));


	init_cells(cells, N, N2);//tworzenie tablicy cells
	graphics(N * A + 9, N2 * A + 9);//tworzenie okna z grafiką
	while (animate(2)) {//pętla animacji
		
		
		draw(cells, N, N2);//rysujemy aktualną tablicę
		
		update(cells, N, N2);//zmieniamy stan komórek
	}
	//usuwamy główną tablicę
	for (int i = 0; i < N; ++i)
		free(cells[i]);
	free(cells);
	//wait();
	return 0;
}