#include <stdio.h>
#include <math.h>
#include "winbgi2.h"


//w tym pliku są funkcję, które były wykorzystywane podczas laboratorium
double norma(double* r, int n){
	double k = 0;
	for (int i = 0; i < n; i++){
		k += r[i] * r[i];
	}
	return sqrt(k);
}


void iloczyn(double **A, double *x, double *b, int n){
	for (int i = 0; i < n; i++){
		b[i] = 0;
		for (int j = 0; j < n; j++){
			b[i] += A[i][j] * x[j];
		}
	}
}

void copy_vector(double* a, double* b, int n) {
	//b = new_vector(n);
	for (int i = 0; i < n; i++) {
		b[i] = a[i];
	}
}

double mult_vec(double* x1, double* x2, int n){
	double k = 0;
	for (int i = 0; i < n; i++){
		k += x1[i] * x2[i];
	}
	return k;
}

void resid(int n, double** A, double* b, double* x, double* r){
	for (int i = 0; i < n; i++){
		double k = 0;
		for (int j = 0; j < n; j++){
			k += A[i][j] * x[j];
		}
		r[i] = b[i] - k;
	}
}


void add_vector_alpha(double* x1, double* x2, double alpha, int n){
	for (int i = 0; i < n; i++){
		x1[i] = x1[i] + x2[i]*alpha;
	}
}


double alpha_grad(int n, double* r, double* p, double** A) {
	double* mian1 = new_vector(n);
	iloczyn(A, p, mian1, n);
	double mian2 = mult_vec(mian1, p, n);
	double licz = mult_vec(r, r, n);
	return licz / mian2;
}

double beta_grad(int n, double* r, double*r1) {
	double licz = mult_vec(r1, r1, n);
	double mian = mult_vec(r, r, n);
	return licz / mian;
}

void gradient(int n, double** A, double* b, double* x) {
	double* r = new_vector(n);
	resid(n, A, b, x, r);
	//printf("\n%lf\n", norma(r, n));
	double* p = new_vector(n);
	double* add = new_vector(n);
	double* r1 = new_vector(n);
	double* resid1 = new_vector(n);
	//graphics(700, 700);
	scale(0, -10, 1000, 10);
	copy_vector(r, p, n);
	double alpha = 0;
	double beta = 0;
	for (int m = 0; m < 10000; m++) {
		alpha = alpha_grad(n, r, p, A);
		add_vector_alpha(x, p, alpha, n);
		iloczyn(A, p, add, n);
		copy_vector(r, r1, n);
		add_vector_alpha(r, add, -alpha, n);
		beta = beta_grad(n, r1, r);
		copy_vector(r, r1, n);
		add_vector_alpha(r1, p, beta, n);
		copy_vector(r1, p, n);
		resid(n, A, b, x, resid1);
		//printf("%d\t%lf\n", m, norma(resid1, n));
		point(m, norma(resid1, n));
		if (norma(resid1, n) < 0.00001) break;
	}
}

void SResid(int n, void(*A)(double*, double*, int), double* b, double* x, double* r) {
	double* y = new_vector(n);
	(*A)(x, y, n);
	printf("%lf\t", norma(x, n));
	for (int i = 0; i < n; i++) {
		r[i] = b[i] - y[i];
	}
	free(y);
}

double SAlpha(int n, double* r, double* p, void(*A)(double*, double*, int)) {
	double* mian1 = new_vector(n);
	(*A)(p, mian1, n);
	double mian2 = mult_vec(mian1, p, n);
	double licz = mult_vec(r, r, n);
	free(mian1);
	return licz / mian2;
}

void SGradient(int n, void(*A)(double*, double*, int), double* b, double* x) {
	double* r = new_vector(n);
	printf("%lf\t", norma(x, n));
	SResid(n, A, b, x, r);
	double* p = new_vector(n);
	double* add = new_vector(n);
	double* r1 = new_vector(n);
	double* resid1 = new_vector(n);
	//graphics(700, 700);
	//scale(0, -10, 1000, 10);
	copy_vector(r, p, n);
	double alpha = 0;
	double beta = 0;
	for (int m = 0; m < 50; m++) {
		alpha = SAlpha(n, r, p, A);
		add_vector_alpha(x, p, alpha, n);
		//iloczyn(A, p, add, n);
		A(p, add, n);
		copy_vector(r, r1, n);
		add_vector_alpha(r, add, -alpha, n);
		beta = beta_grad(n, r1, r);
		copy_vector(r, r1, n);
		add_vector_alpha(r1, p, beta, n);
		copy_vector(r1, p, n);
		//resid(n, A, b, x, resid1);
		SResid(n, A, b, x, resid1);
		printf("%d\t%lf\n", m, norma(resid1, n));
		//point(m, norma(resid1, n));
		if (norma(resid1, n) < 0.00001) break;
	}
}
