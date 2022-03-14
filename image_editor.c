//Copyright 2022 Ilinca Sebastian-Ionut 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
//NOTE: coordonates in order after load are find on the last and pre-last place
//of struct(x_c and y_c)
typedef double dbl;
typedef unsigned char uc;

struct rgb {
	int r, g, b;
};

typedef struct rgb rgb;

//struct for the filters, this struct contains all of
//vectors of vectors needed
struct k_fil {
	dbl edge[3][3];
	dbl shar[3][3];
	dbl blur[3][3];
	dbl gsblur[3][3];
};

typedef struct k_fil kfil;

//the mood itself from the two structures represent the condition
//of the program, 0 stands for the fact that no image is loaded
//1 -  stands for image loaded
//structure for w/b
struct pgm {
	char *magic_word;
	int height, width;
	int MAXVAL;
	uc **pix;
	int x_c[4];
	int y_c[4];
	int mood;
};

//structure for colr
struct ppm {
	char *magic_word;
	int height, width;
	int MAXVAL;
	rgb **pix;
	int x_c[4];
	int y_c[4];
	int mood;
};

typedef struct pgm pgm;
typedef struct ppm ppm;

//remove the newline from the string
void remove_basic_newline(char *s)
{
	size_t len = strlen(s);
	if (len > 0 && s[len - 1] == '\n')
		s[len - 1] = 0;
}

//dynamic alloc for the strings
void alloc_dynamic_char(char **s, char **s1)
{
	*s = malloc(100 * sizeof(char));
	if (!*s) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	*s1 = malloc(100 * sizeof(char));
	if (!*s1) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
}

//a function to make me figure out what command is about
void read_and_select(char *s, char *s1)
{
	fgets(s, 100, stdin);
	remove_basic_newline(s);
	strcpy(s1, s);
	s1 = strtok(s1, " ");
}

void free_that(pgm *grey, ppm *colr)
{
	//free the values allocated dynamic in the program
	//depends on the mood
	if (grey->mood == 1) {
		for (int i = 0; i < grey->height; ++i)
			free(grey->pix[i]);
		free(grey->pix);
		free(grey->magic_word);
		grey->height = 0;
		grey->width = 0;
		grey->MAXVAL = 0;
		grey->mood = 0;
	}
	if (colr->mood == 1) {
		for (int i = 0; i < colr->height; ++i)
			free(colr->pix[i]);
		free(colr->pix);
		free(colr->magic_word);
		colr->height = 0;
		colr->width = 0;
		colr->MAXVAL = 0;
		colr->mood = 0;
	}
}

void attribute_grey(pgm *grey, int w, int h, int max)
{
	//keep the values memorized and usable
	grey->height = h;
	grey->width = w;
	grey->MAXVAL = max;
	grey->mood = 1;
	grey->x_c[2] = 0;
	grey->x_c[3] = w;
	grey->y_c[2] = 0;
	grey->y_c[3] = h;
}

void attribute_colr(ppm *colr, int w, int h, int max)
{
	//keep the values memorized and usable
	colr->height = h;
	colr->width = w;
	colr->MAXVAL = max;
	colr->mood = 1;
	colr->x_c[2] = 0;
	colr->x_c[3] = w;
	colr->y_c[2] = 0;
	colr->y_c[3] = h;
}

void shortcut_colr(ppm *colr, int h, int w, int max, char *c)
{
	//alloc size
	colr->magic_word = malloc(4 * sizeof(char));
	colr->pix = malloc(h * sizeof(rgb *));
	if (!colr->magic_word || !colr->pix) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	for (int i = 0; i < h; ++i) {
		colr->pix[i] = malloc(w * sizeof(rgb));
		if (!colr->pix[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	strcpy(colr->magic_word, c);
	attribute_colr(colr, w, h, max);
}

void shortcut_grey(pgm *grey, int h, int w, int max, char *c)
{
	grey->magic_word = malloc(4 * sizeof(char));
	grey->pix = malloc(h * sizeof(uc *));
	if (!grey->magic_word || !grey->pix) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	strcpy(grey->magic_word, c);
	for (int i = 0; i < h; ++i) {
		grey->pix[i] = malloc(w * sizeof(uc));
		if (!grey->pix[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	attribute_grey(grey, w, h, max);
}

//loading the file in the memory's program
void load_image(FILE *image, char *com, char *buf, pgm *grey, ppm *colr)
{
	int h, w, max;
	buf = strtok(com, " ");
	buf = strtok(NULL, " ");
	if (grey->mood >= 1 || colr->mood >= 1)
		free_that(grey, colr);
	image = fopen(buf, "rt");
	if (!image) {
		printf("Failed to load %s\n", buf);
		return;
	}
	char *c;
	c = malloc(4 * sizeof(char));
	fscanf(image, "%s %d %d %d", c, &w, &h, &max);
	if (c[1] == '2' || c[1] == '3') {
		if (c[1] == '2') {
			shortcut_grey(grey, h, w, max, c);
			for (int i = 0; i < h; ++i) {
				for (int j = 0; j < w; ++j)
					fscanf(image, "%hhd", &grey->pix[i][j]);
			}
		}
		if (c[1] == '3') {
			shortcut_colr(colr, h, w, max, c);
			for (int i = 0; i < h; ++i) {
				for (int j = 0; j < w; ++j) {
					fscanf(image, "%d", &((colr->pix[i][j])).r);
					fscanf(image, "%d", &((colr->pix[i][j])).g);
					fscanf(image, "%d", &((colr->pix[i][j])).b);
					colr->pix[i][j].r *= 1000;
					colr->pix[i][j].g *= 1000;
					colr->pix[i][j].b *= 1000;
				}
			}
		}
	}
	if (c[1] == '5' || c[1] == '6') {
		int aux = ftell(image);
		fclose(image);
		image = fopen(buf, "rb");
		fseek(image, aux, SEEK_SET);
		fseek(image, 1, SEEK_CUR);
		if (c[1] == '5') {
			shortcut_grey(grey, h, w, max, c);
			for (int i = 0; i < h; ++i) {
				for (int j = 0; j < w; ++j)
					fread(&grey->pix[i][j], sizeof(uc), 1, image);
			}
		}
		if (c[1] == '6') {
			shortcut_colr(colr, h, w, max, c);
			uc r, g, b;
			for (int i = 0; i < h; ++i) {
				for (int j = 0; j < w; ++j) {
					fread(&r, sizeof(uc), 1, image);
					fread(&g, sizeof(uc), 1, image);
					fread(&b, sizeof(uc), 1, image);
					colr->pix[i][j].r = r * 1000;
					colr->pix[i][j].g = g * 1000;
					colr->pix[i][j].b = b * 1000;
				}
			}
		}
	}
	free(c);
	fclose(image);
	printf("Loaded %s\n", buf);
}

void actions_g(pgm *grey)
{
	//have the values of the main matrix
	grey->x_c[3] = grey->width;
	grey->x_c[2] = 0;
	grey->y_c[3] = grey->height;
	grey->y_c[2] = 0;
}

void actions_c(ppm *colr)
{
	//the coords now have the values of the entire matrix
	colr->x_c[3] = colr->width;
	colr->x_c[2] = 0;
	colr->y_c[3] = colr->height;
	colr->y_c[2] = 0;
}

//decide if the matrix for select all is greyscale or colr
void decide(pgm *grey, ppm *colr)
{
	if (grey->mood > 0)
		actions_g(grey);
	if (colr->mood > 0)
		actions_c(colr);
}

//1 for greyscale, 2 for colr
int decide1(pgm *grey, ppm *colr)
{
	if (grey->mood > 0)
		return 1;
	if (colr->mood > 0)
		return 2;
	return -1;
}

//make the selects
void sel_grey_co(pgm *grey, int x1, int x2, int y1, int y2)
{
	if (x1 > grey->width || x2 > grey->width) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (y1 > grey->height || y2 > grey->height) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 < 0 || x2 < 0) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (y1 < 0 || y2 < 0) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	//put in the first elements the coordonates as they come in
	//for the print
	grey->x_c[0] = x1;
	grey->x_c[1] = x2;
	grey->y_c[0] = y1;
	grey->y_c[1] = y2;
	//a little sort for efficiency of the program
	//and easiness to work
	if (x1 < x2) {
		grey->x_c[2] = x1;
		grey->x_c[3] = x2;
	}
	if (x2 <= x1) {
		grey->x_c[2] = x2;
		grey->x_c[3] = x1;
	}
	if (y1 < y2) {
		grey->y_c[2] = y1;
		grey->y_c[3] = y2;
	}
	if (y2 <= y1) {
		grey->y_c[2] = y2;
		grey->y_c[3] = y1;
	}
	//print a message to make the people know
	//that the selection was a success
	printf("Selected %d %d ", grey->x_c[2], grey->y_c[2]);
	printf("%d %d\n", grey->x_c[3], grey->y_c[3]);
}

void sel_colr_co(ppm *colr, int x1, int x2, int y1, int y2)
{
	// printf("%d %d %d %d", colr->height, colr->width, x2, y2);
	if (x1 > colr->width || x2 > colr->width) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (y1 > colr->height || y2 > colr->height) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 < 0 || x2 < 0) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (y1 < 0 || y2 < 0) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	//for the print I decided to put
	//the coordonates as they come in
	colr->x_c[0] = x1;
	colr->x_c[1] = x2;
	colr->y_c[0] = y1;
	colr->y_c[1] = y2;
	//I sort the coordonates for efficiency
	//of the program
	if (x1 < x2) {
		colr->x_c[2] = x1;
		colr->x_c[3] = x2;
	}
	if (x2 <= x1) {
		colr->x_c[2] = x2;
		colr->x_c[3] = x1;
	}
	if (y1 < y2) {
		colr->y_c[2] = y1;
		colr->y_c[3] = y2;
	}
	if (y2 <= y1) {
		colr->y_c[2] = y2;
		colr->y_c[3] = y1;
	}
	//print a message to make the people know
	//that the selection was a success
	printf("Selected %d %d ", colr->x_c[2], colr->y_c[2]);
	printf("%d %d\n", colr->x_c[3], colr->y_c[3]);
}

//verify if it s a number or the command
//if it is "SELECT ALL"
int is_it_number(char *buffer, char *fel)
{
	int b = 0;
	fel = strtok(buffer, " ");
	fel = strtok(NULL, " ");
	if (!fel)
		return -1;
	if (strcmp(fel, "ALL") == 0)
		return 0;
	int n = strlen(fel);
	for (int i = 0; i < n; ++i) {
		if (isdigit(fel[i]) == 0 && fel[i] != '-')
			b = -1;
	}
	fel = strtok(NULL, " ");
	if (!fel)
		return -1;
	n = strlen(fel);
	for (int i = 0; i < n; ++i) {
		if (isdigit(fel[i]) == 0 && fel[i] != '-')
			b = -1;
	}
	fel = strtok(NULL, " ");
	if (!fel)
		return -1;
	n = strlen(fel);
	for (int i = 0; i < n; ++i) {
		if (isdigit(fel[i]) == 0 && fel[i] != '-')
			b = -1;
	}
	fel = strtok(NULL, " ");
	if (!fel)
		return -1;
	n = strlen(fel);
	for (int i = 0; i < n; ++i) {
		if (isdigit(fel[i]) == 0 && fel[i] != '-')
			b = -1;
	}
	return b;
}

//this functions send the parameters to other
//functions depending on
void selection_op(char *com, char *buf, pgm *grey, ppm *colr)
{
	char *trash;
	trash = malloc(100 * sizeof(char));
	if (!trash) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	char *th;
	th = malloc(100 * sizeof(char));
	if (!th) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	//if it is not a number and is not all
	//surely is an invalid command
	strcpy(trash, com);
	int itis = is_it_number(trash, th);
	if (itis == -1) {
		printf("Invalid command\n");
		free(trash);
		free(th);
		return;
	}
	free(trash);
	free(th);
	if (colr->mood == 0 && grey->mood == 0) {
		printf("No image loaded\n");
		return;
	}
	//condition to select all
	buf = strtok(com, " ");
	buf = strtok(NULL, " ");
	if (strcmp(buf, "ALL") == 0) {
		decide(grey, colr);
		printf("Selected ALL\n");
	}
	if (strcmp(buf, "ALL") != 0) {
		//take into the variables the coordonates
		int x1 = atoi(buf);
		buf = strtok(NULL, " ");
		int y1 = atoi(buf);
		buf = strtok(NULL, " ");
		int x2 = atoi(buf);
		buf = strtok(NULL, " ");
		int y2 = atoi(buf);
		//the decision helps me realise what
		//image to select, the type
		//of image to be more clear
		int decision = decide1(grey, colr);
		if (decision == 1)
			sel_grey_co(grey, x1, x2, y1, y2);
		if (decision == 2)
			sel_colr_co(colr, x1, x2, y1, y2);
	}
}

//verify if the working matrix on rotate
//operation is a square or is the wholo photo
int verify_grey(pgm *grey)
{
	//a-collumns, b - lines
	int a = grey->x_c[3] - grey->x_c[2];
	int b = grey->y_c[3] - grey->y_c[2];
	if (a == grey->width && b == grey->height)
		return 1;
	if (a == b)
		return 1;
	if (a != grey->width || b != grey->height)
		return -1;
	return 100;
}

int verify_colr(ppm *colr)
{
	//a-colluns, b - lines
	int a = colr->x_c[3] - colr->x_c[2];
	int b = colr->y_c[3] - colr->y_c[2];
	if (a == colr->width && b == colr->height)
		return 1;
	if (a == b)
		return 1;
	if (a != colr->width || b != colr->height)
		return -1;
	return 1000;
}

//check if the angle is supported
int verify_angle(char *com, char *buf, int *q)
{
	buf = strtok(com, " ");
	buf = strtok(NULL, " ");
	*q = atoi(buf);
	if (*q == 360 || *q == -360)
		return 1;
	if (*q == 270 || *q == -270)
		return 1;
	if (*q == 180 || *q == -180)
		return 1;
	if (*q == 90 || *q == -90)
		return 1;
	if (*q == 0)
		return 1;
	return -1;
}

void cp_and_r(uc **mat, pgm *grey, int a)
{
	//copy values
	for (int i = 0; i < a; ++i) {
		for (int j = 0; j < a; ++j)
			mat[i][j] = grey->pix[i + grey->y_c[2]][j + grey->x_c[2]];
	}
	//90 degrees
	for (int i = 0; i < a / 2; ++i) {
		for (int j = i; j < a - i - 1; ++j) {
			int tmp = mat[i][j];
			mat[i][j] = mat[a - 1 - j][i];
			mat[a - 1 - j][i] = mat[a - 1 - i][a - 1 - j];
			mat[a - 1 - i][a - 1 - j] = mat[j][a - 1 - i];
			mat[j][a - 1 - i] = tmp;
		}
	}
	//put the values back
	for (int i = 0; i < a; ++i) {
		for (int j = 0; j < a; ++j)
			grey->pix[i + grey->y_c[2]][j + grey->x_c[2]] = mat[i][j];
	}
}

int maximum(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

void hard_w(uc **mat, pgm *grey, int max)
{
	//copy the values from the main matrix
	for (int i = 0; i < grey->height; ++i) {
		for (int j = 0; j < grey->width; ++j)
			mat[i][j] = grey->pix[i][j];
	}
	int aux, i = 0;
	while (i < max) {
		for (int k = i; k < max; ++k) {
			aux = mat[k][i];
			mat[k][i] = mat[i][k];
			mat[i][k] = aux;
		}
		++i;
	}
}

void swap_c(pgm *grey, uc **mat)
{
	//swap the collumns
	//to be rotated to 90 degrees
	for (int k = 0; k < grey->height; ++k) {
		for (int j = 0; j < grey->width / 2; ++j) {
			int temp = mat[k][j];
			mat[k][j] = mat[k][grey->width - 1 - j];
			mat[k][grey->width - 1 - j] = temp;
		}
	}
	//copy the values into the matrix;
	for (int k = 0; k < grey->height; ++k) {
		for (int j = 0; j < grey->width; ++j)
			grey->pix[k][j] = mat[k][j];
	}
}

void r_plus_90(pgm *grey)
{
	//a-collumns, b - lines
	int a = (grey->x_c[3] - grey->x_c[2]);
	int b = (grey->y_c[3] - grey->y_c[2]);
	if (a == b) {
		uc **mat;
		//alloc
		mat = malloc(a * sizeof(uc *));
		if (!mat) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
		for (int i = 0; i < a; ++i) {
			mat[i] = malloc(a * sizeof(uc));
			if (!mat[i]) {
				fprintf(stderr, "Malloc failed\n");
				return;
			}
		}
		cp_and_r(mat, grey, a);
		//free
		for (int i = 0; i < a; ++i)
			free(mat[i]);
		free(mat);
	}
	if (a != b) {
		uc **mat;
		int max = maximum(a, b);
		//alloc to be like a NxN matrix
		mat = malloc(max * sizeof(uc *));
		if (!mat) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
		for (int i = 0; i < max; ++i) {
			mat[i] = malloc(max * sizeof(uc));
			if (!mat[i]) {
				fprintf(stderr, "Malloc failed\n");
				return;
			}
		}
		hard_w(mat, grey, max);
		int aux = a;
		a = b;
		b = aux;
		grey->x_c[3] = a;
		grey->y_c[3] = b;
		for (int k = 0; k < grey->height; ++k)
			free(grey->pix[k]);
		free(grey->pix);
		grey->height = grey->y_c[3];
		grey->width = grey->x_c[3];
		grey->pix = malloc(grey->height * sizeof(uc *));
		if (!grey->pix) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
		for (int k = 0; k < grey->height; ++k) {
			grey->pix[k] = malloc(grey->width * sizeof(uc));
			if (!grey->pix[k]) {
				fprintf(stderr, "Malloc failed\n");
				return;
			}
		}
		swap_c(grey, mat);
		for (int i = 0; i < max; ++i)
			free(mat[i]);
		free(mat);
	}
}

//these two functions actually use the 90 degrees rotate
//because 180 == 90 * 2 and 270 = 90 * 3
void r_180(pgm *grey)
{
	for (int i = 0; i < 2; ++i)
		r_plus_90(grey);
}

void r_m90_p270(pgm *grey)
{
	for (int i = 0; i < 3; ++i)
		r_plus_90(grey);
}

void decide_rotate_angle_g(int q, pgm *grey)
{
	//sends to functions the argument
	//and shows a message
	int z = q;
	if (z == 360)
		printf("Rotated 360\n");
	if (z == -360)
		printf("Rotated -360\n");
	if (z == 180) {
		r_180(grey);
		printf("Rotated 180\n");
	}
	if (z == -180) {
		r_180(grey);
		printf("Rotated -180\n");
	}
	if (z == -270) {
		r_plus_90(grey);
		printf("Rotated -270\n");
	}
	if (z == 90) {
		r_plus_90(grey);
		printf("Rotated 90\n");
	}
	if (z == 270) {
		r_m90_p270(grey);
		printf("Rotated 270\n");
	}
	if (z == -90) {
		r_m90_p270(grey);
		printf("Rotated -90\n");
	}
	if (z == 0)
		printf("Rotated 0\n");
}

void fake_it_make_it(rgb **mat, ppm *colr, int a)
{
	for (int i = 0; i < a; ++i) {
		for (int j = 0; j < a; ++j) {
			mat[i][j].r = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].r;
			mat[i][j].g = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].g;
			mat[i][j].b = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].b;
		}
	}
	for (int i = 0; i < a / 2; ++i) {
		for (int j = i; j < a - i - 1; ++j) {
			int tmpr = mat[i][j].r;
			mat[i][j].r = mat[a - 1 - j][i].r;
			mat[a - 1 - j][i].r = mat[a - 1 - i][a - 1 - j].r;
			mat[a - 1 - i][a - 1 - j].r = mat[j][a - 1 - i].r;
			mat[j][a - 1 - i].r = tmpr;
			int tmpg = mat[i][j].g;
			mat[i][j].g = mat[a - 1 - j][i].g;
			mat[a - 1 - j][i].g = mat[a - 1 - i][a - 1 - j].g;
			mat[a - 1 - i][a - 1 - j].g = mat[j][a - 1 - i].g;
			mat[j][a - 1 - i].g = tmpg;
			int tmpb = mat[i][j].b;
			mat[i][j].b = mat[a - 1 - j][i].b;
			mat[a - 1 - j][i].b = mat[a - 1 - i][a - 1 - j].b;
			mat[a - 1 - i][a - 1 - j].b = mat[j][a - 1 - i].b;
			mat[j][a - 1 - i].b = tmpb;
		}
	}
	for (int i = 0; i < a; ++i) {
		for (int j = 0; j < a; ++j) {
			colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].r = mat[i][j].r;
			colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].g = mat[i][j].g;
			colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].b = mat[i][j].b;
		}
	}
}

int maxi(int a, int b)
{
	if (a > b)
		return a;
	return b;
}

void come_on(rgb **mat, ppm *colr, int max)
{
	for (int i = 0; i < colr->height; ++i) {
		for (int j = 0; j < colr->width; ++j) {
			mat[i][j].r = colr->pix[i][j].r;
			mat[i][j].g = colr->pix[i][j].g;
			mat[i][j].b = colr->pix[i][j].b;
		}
	}
	int aux, i = 0;
	while (i < max) {
		for (int k = i; k < max; ++k) {
			aux = mat[k][i].r;
			mat[k][i].r = mat[i][k].r;
			mat[i][k].r = aux;
			aux = mat[k][i].g;
			mat[k][i].g = mat[i][k].g;
			mat[i][k].g = aux;
			aux = mat[k][i].b;
			mat[k][i].b = mat[i][k].b;
			mat[i][k].b = aux;
		}
		++i;
	}
}

void too_much_functions(rgb **mat, ppm *colr)
{
	//swap the collumns to
	//be rotated by 90 degrees
	for (int k = 0; k < colr->height; ++k) {
		for (int j = 0; j < colr->width / 2; ++j) {
			dbl temp = mat[k][j].r;
			mat[k][j].r = mat[k][colr->width - 1 - j].r;
			mat[k][colr->width - 1 - j].r = temp;
			temp = mat[k][j].g;
			mat[k][j].g = mat[k][colr->width - 1 - j].g;
			mat[k][colr->width - 1 - j].g = temp;
			temp = mat[k][j].b;
			mat[k][j].b = mat[k][colr->width - 1 - j].b;
			mat[k][colr->width - 1 - j].b = temp;
		}
	}
	//copy the values to the new main
	//made matrix
	for (int k = 0; k < colr->height; ++k) {
		for (int j = 0; j < colr->width; ++j) {
			colr->pix[k][j].r = mat[k][j].r;
			colr->pix[k][j].g = mat[k][j].g;
			colr->pix[k][j].b = mat[k][j].b;
		}
	}
}

void rotate_90p_c(ppm *colr)
{
	//a for collumns, b for lines
	int a  = (colr->x_c[3] - colr->x_c[2]);
	int b = (colr->y_c[3] - colr->y_c[2]);
	if (a == b) {
		rgb **mat;
		mat = malloc(a * sizeof(rgb *));
		if (!mat) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
		for (int i = 0; i < a; ++i) {
			mat[i] = malloc(a * sizeof(rgb));
			if (!mat[i]) {
				fprintf(stderr, "Malloc failed\n");
				return;
			}
		}
		fake_it_make_it(mat, colr, a);
		for (int i = 0; i < a; ++i)
			free(mat[i]);
		free(mat);
	}
	if (a != b) {
		rgb **mat;
		//a - collumns, b - lines
		//we are going to make a trasnpose turning
		//the matrix into a square then release the space
		//and interchange collumns
		int max = maxi(a, b);
		//alloc the necessary space
		mat = malloc(max * sizeof(rgb *));
		if (!mat) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
		for (int i = 0; i < max; ++i) {
			mat[i] = malloc(max * sizeof(rgb));
			if (!mat[i]) {
				fprintf(stderr, "Malloc failed\n");
				return;
			}
		}
		come_on(mat, colr, max);
		int aux = a;
		a = b;
		b = aux;
		//invers
		colr->x_c[3] = a;
		colr->y_c[3] = b;
		//free main matrix because
		//it s the whole photo bcs
		//it s not a square
		for (int k = 0; k < colr->height; ++k)
			free(colr->pix[k]);
		free(colr->pix);
		//change the sizes
		colr->height = colr->y_c[3];
		colr->width = colr->x_c[3];
		//alloc
		colr->pix = malloc(colr->height * sizeof(rgb *));
		if (!colr->pix) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
		for (int k = 0; k < colr->height; ++k) {
			colr->pix[k] = malloc(colr->width * sizeof(rgb));
			if (!colr->pix[k]) {
				fprintf(stderr, "Malloc failed\n");
				return;
			}
		}
		too_much_functions(mat, colr);
		for (int i = 0; i < max; ++i)
			free(mat[i]);
		free(mat);
	}
}

void rot_180(ppm *colr)
{
	for (int i = 0; i < 2; ++i)
		rotate_90p_c(colr);
}

void rot_270(ppm *colr)
{
	for (int i = 0; i < 3; ++i)
		rotate_90p_c(colr);
}

void decide_rotate_c(int q, ppm *colr)
{
	//send the paramter to the functions
	//accordingly with the degree wanted
	int z = q;
	if (z == 90) {
		rotate_90p_c(colr);
		printf("Rotated 90\n");
	}
	if (z == -270) {
		rotate_90p_c(colr);
		printf("Rotated -270\n");
	}
	if (z == 180) {
		rot_180(colr);
		printf("Rotated 180\n");
	}
	if (z == -180) {
		rot_180(colr);
		printf("Rotated -180\n");
	}
	if (z == 270) {
		rot_270(colr);
		printf("Rotated 270\n");
	}
	if (z == -90) {
		rot_270(colr);
		printf("Rotated -90\n");
	}
	if (z == 360)
		printf("Rotated 360\n");
	if (z == -360)
		printf("Rotated -360\n");
	if (z == 0)
		printf("Rotated 0\n");
}

void rotate_colr(char *com, char *buf, ppm *colr)
{
	//check if the selection is a square or a whole photo
	//or if it's an unsupported angle
	int q;
	int c = verify_colr(colr);
	if (c == -1) {
		printf("The selection must be square\n");
		return;
	}
	int u = verify_angle(com, buf, &q);
	if (u == -1) {
		printf("Unsupported rotation angle\n");
		return;
	}
	decide_rotate_c(q, colr);
}

void rotate_grey(char *com, char *buf, pgm *grey)
{
	//verify if it is the good angle or it s the perfect size
	//to rotate
	int q;
	int c = verify_grey(grey);
	if (c == -1) {
		printf("The selection must be square\n");
		return;
	}
	int u = verify_angle(com, buf, &q);
	if (u == -1) {
		printf("Unsupported rotation angle\n");
		return;
	}
	decide_rotate_angle_g(q, grey);
}

void rotate_op(char *com, char *buf, pgm *grey, ppm *colr)
{
	//sends to different functions accordingly with the mood
	if (colr->mood == 0 && grey->mood == 0) {
		printf("No image loaded\n");
		return;
	}
	if (grey->mood == 1)
		rotate_grey(com, buf, grey);
	if (colr->mood == 1)
		rotate_colr(com, buf, colr);
}

void crop_colr(ppm *colr)
{
	//a-collumns and b -lines
	int a = colr->x_c[3] - colr->x_c[2];
	int b = colr->y_c[3] - colr->y_c[2];
	//in this case no operations needed
	if (a == colr->width && b == colr->height) {
		printf("Image cropped\n");
		return;
	}
	rgb **mat;
	mat = malloc(b * sizeof(rgb *));
	if (!mat) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	for (int i = 0; i < b; ++i) {
		mat[i] = malloc(a * sizeof(rgb));
		if (!mat[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	for (int i = 0; i < b; ++i) {
		for (int j = 0; j < a; ++j) {
			mat[i][j].r = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].r;
			mat[i][j].g = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].g;
			mat[i][j].b = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].b;
		}
	}
	//free the matrix
	for (int i = 0; i < colr->height; ++i)
		free(colr->pix[i]);
	free(colr->pix);
	//new dimensions
	colr->height = b;
	colr->width = a;
	//alloc memory
	colr->pix = malloc(b * sizeof(rgb *));
	if (!colr->pix) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	for (int i = 0; i < b; ++i) {
		colr->pix[i] = malloc(a * sizeof(rgb));
		if (!colr->pix[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	//write down the new coordinates
	colr->x_c[2] = 0;
	colr->x_c[3] = a;
	colr->y_c[2] = 0;
	colr->y_c[3] = b;
	//copy the values
	for (int i = 0; i < b; ++i) {
		for (int j = 0; j < a; ++j) {
			colr->pix[i][j].r = mat[i][j].r;
			colr->pix[i][j].g = mat[i][j].g;
			colr->pix[i][j].b = mat[i][j].b;
		}
	}
	for (int i = 0; i < b; ++i)
		free(mat[i]);
	free(mat);
	printf("Image cropped\n");
}

void crop_grey(pgm *grey)
{
	//a-collumns b-lines
	int a = grey->x_c[3] - grey->x_c[2];
	int b = grey->y_c[3] - grey->y_c[2];
	if (a == grey->width && b == grey->height) {
		printf("Image cropped\n");
		return;
	}
	uc **mat;
	//alloc and copy
	mat = malloc(b * sizeof(uc *));
	if (!mat) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	for (int i = 0; i < b; ++i) {
		mat[i] = malloc(a * sizeof(uc));
		if (!mat[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	for (int i = 0; i < b; ++i) {
		for (int j = 0; j < a; ++j)
			mat[i][j] = grey->pix[i + grey->y_c[2]][j + grey->x_c[2]];
	}
    //free
	for (int i = 0; i < grey->height; ++i)
		free(grey->pix[i]);
	free(grey->pix);
	//alloc the new matrix
	grey->pix = malloc(b * sizeof(uc *));
	for (int i = 0; i < b; ++i)
		grey->pix[i] = malloc(a * sizeof(uc));
	//copy the values
	for (int i = 0; i < b; ++i) {
		for (int j = 0; j < a; ++j)
			grey->pix[i][j] = mat[i][j];
	}
	/*for (int i = 0; i < a; ++i) {
		for (int j = 0; j < b; ++j) {
			printf("%d ",grey->pix[i][j]);
		}
	}*/
	//attribute the new values
	grey->height = b;
	grey->width = a;
	grey->x_c[3] = a;
	grey->x_c[2] = 0;
	grey->y_c[3] = b;
	grey->y_c[2] = 0;
	for (int i = 0; i < b; ++i)
		free(mat[i]);
	free(mat);
	printf("Image cropped\n");
}

//decides and sends to functions accordingly
void crop_op(pgm *grey, ppm *colr)
{
	if (colr->mood == 0 && grey->mood == 0)
		printf("No image loaded\n");
	if (grey->mood == 1)
		crop_grey(grey);
	if (colr->mood == 1)
		crop_colr(colr);
}

//1 for ascii, 0 for binary
int is_it_ascii(char *com, char *buf, char *tamp)
{
	buf = strtok(com, " ");
	buf = strtok(NULL, " ");
	strcpy(tamp, buf);
	buf = strtok(NULL, " ");
	if (!buf)
		return 0;
	if (strcmp(buf, "ascii") == 0)
		return 1;
	return 0;
}

void sav_txt_g(char *buf, pgm *grey, FILE *image)
{
	if (grey->magic_word[1] == '5')
		grey->magic_word[1] = '2';
	image = fopen(buf, "wt");
	if (!image) {
		printf("FAILED\n");
		return;
	}
	//print first elements
	fprintf(image, "%s\n", grey->magic_word);
	fprintf(image, "%d %d\n", grey->width, grey->height);
	fprintf(image, "%d\n", grey->MAXVAL);
	for (int i = 0; i < grey->height; ++i) {
		for (int j = 0; j < grey->width; ++j) {
			//print the values
			fprintf(image, "%hhu ", grey->pix[i][j]);
		}
		fprintf(image, "\n");
	}
	fclose(image);
	printf("Saved %s\n", buf);
}

int my_round(dbl a)
{
	a += 0.5;
	int b = (int)a;
	return b;
}

void sav_txt_c(char *buf, ppm *colr, FILE *image)
{
	if (colr->magic_word[1] == '6')
		colr->magic_word[1] = '3';
	image = fopen(buf, "wt");
	if (!image) {
		fprintf(stderr, "FAILED\n");
		return;
	}
	//print first elements
	fprintf(image, "%s\n", colr->magic_word);
	fprintf(image, "%d %d\n", colr->width, colr->height);
	fprintf(image, "%d\n", colr->MAXVAL);
	uc p, l, m;
	for (int i = 0; i < colr->height; ++i) {
		for (int j = 0; j < colr->width; ++j) {
			//casting the values in uc
			p = round(1.0 * colr->pix[i][j].r / 1000);
			l = round(1.0 * colr->pix[i][j].g / 1000);
			m = round(1.0 * colr->pix[i][j].b / 1000);
			fprintf(image, "%hhu ", p);
			fprintf(image, "%hhu ", l);
			fprintf(image, "%hhu ", m);
		}
		fprintf(image, "\n");
	}
	fclose(image);
	printf("Saved %s\n", buf);
}

//for saving binary
void sav_bin_g(char *buf, pgm *grey, FILE *image)
{
	//if the file I opened
	//to read was ascii I transform
	//the magic word for binary
	if (grey->magic_word[1] == '2')
		grey->magic_word[1] = '5';
	image = fopen(buf, "wt");
	if (!image) {
		printf("FAILED\n");
		return;
	}
	//print like text first elements to can
	//be readable by another or this program
	fprintf(image, "%s\n", grey->magic_word);
	fprintf(image, "%d %d\n", grey->width, grey->height);
	fprintf(image, "%d\n", grey->MAXVAL);
	fclose(image);
	image = fopen(buf, "ab");
	if (!image) {
		printf("FAILED\n");
		return;
	}
	//write in binary the values
	for (int i = 0; i < grey->height; ++i) {
		for (int j = 0; j < grey->width; ++j)
			fwrite(&grey->pix[i][j], sizeof(uc), 1, image);
	}
	fclose(image);
	printf("Saved %s\n", buf);
}

void sav_bin_c(char *buf, ppm *colr, FILE *image)
{
	//if it is text, save it binary
	if (colr->magic_word[1] == '3')
		colr->magic_word[1] = '6';
	image = fopen(buf, "wt");
	if (!image) {
		fprintf(stderr, "FAILED\n");
		return;
	}
	//print like text first elements to can
	//be readable by another or this program
	fprintf(image, "%s\n", colr->magic_word);
	fprintf(image, "%d %d\n", colr->width, colr->height);
	fprintf(image, "%d\n", colr->MAXVAL);
	fclose(image);
	image = fopen(buf, "ab");
	if (!image) {
		fprintf(stderr, "FAILED\n");
		return;
	}
	// printf("%d %d\n", colr->height, colr->width);
	for (int i = 0; i < colr->height; ++i) {
		for (int j = 0; j < colr->width; ++j) {
			//uc because it s binary and the files need
			//to contain values max 255
			//represented by 1 byte
			uc p = round(1.0 * colr->pix[i][j].r / 1000);
			uc l = round(1.0 * colr->pix[i][j].g / 1000);
			uc m = round(1.0 * colr->pix[i][j].b / 1000);
			fwrite(&p, sizeof(uc), 1, image);
			fwrite(&l, sizeof(uc), 1, image);
			fwrite(&m, sizeof(uc), 1, image);
		}
	}
	fclose(image);
	printf("Saved %s\n", buf);
}

//function to decide what save needs to be done
void save(char *com, char *buf, pgm *grey, ppm *colr, FILE *image)
{
	char tamp[100];
	if (grey->mood == 0 && colr->mood == 0) {
		printf("No image loaded\n");
		return;
	}
	//because if the color is greyscale or color
	//and if the file you want to save is ascii or binary
	int v = is_it_ascii(com, buf, tamp);
	if (v == 1 && grey->mood == 1)
		sav_txt_g(tamp, grey, image);
	if (v == 1 && colr->mood == 1)
		sav_txt_c(tamp, colr, image);
	if (v == 0 && grey->mood == 1)
		sav_bin_g(tamp, grey, image);
	if (v == 0 && colr->mood == 1)
		sav_bin_c(tamp, colr, image);
}

//detects what filter should be applied
int is_okayp(char *com, char *buf)
{
	//this function returns a number
	//which has a connection with the calculus I return
	buf = strtok(com, " ");
	buf = strtok(NULL, " ");
	if (!buf)
		return -1;
    // printf("%s\n", buf);
	if (strcmp(buf, "EDGE") == 0)
		return 1;
	if (strcmp(buf, "SHARPEN") == 0)
		return 2;
	if (strcmp(buf, "BLUR") == 0)
		return 3;
	if (strcmp(buf, "GAUSSIAN_BLUR") == 0)
		return 4;
	return 0;
}

//verify for neighbours
int cond(int i, int j, ppm *colr)
{
	//this function tell me if I have
	//the permision to edit the pixels
	//in order to apply the calculus for a filter
	if ((i - 1) < 0 || (i + 1) >= colr->height)
		return -1;
	if ((j - 1) < 0 || (j + 1) >= colr->width)
		return -1;
	return 0;
}

dbl rr(dbl e)
{
	//it is just a clamp, my clump
	//rr comes from round, it rounds me if the number is too big or
	//or too small a double number which is similar to an integer
	if (e < 0)
		return 0;
	int ad = 255000;
	if (e > ad)
		return ad;
	return e;
}

//operations for the red filter
//in all the filters
dbl final_pixel_red(int i, int j, ppm *colr, kfil *kern, int aux)
{
	dbl e = 0;
	dbl s = 0;
	dbl b = 0;
	dbl g = 0;
	for (int k = i - 1; k < i + 2; ++k) {
		for (int q = j - 1; q < j + 2; ++q) {
			e += colr->pix[k][q].r * kern->edge[k - i + 1][q - j + 1];
			s += colr->pix[k][q].r * kern->shar[k - i + 1][q - j + 1];
			b += colr->pix[k][q].r * kern->blur[k - i + 1][q - j + 1];
			g += colr->pix[k][q].r * kern->gsblur[k - i + 1][q - j + 1];
		}
	}
	if (aux == 1) {
		e /= 10000;
		e = rr(e);
		return e;
	}
	if (aux == 2) {
		s /= 10000;
		s = rr(s);
		return s;
	}
	if (aux == 3) {
		b /= 10000;
		b = rr(b);
		return b;
	}
	if (aux == 4) {
		g /= 10000;
		g = rr(g);
		return g;
	}
	return 10000;
}

//operations for green pixels
dbl final_pixel_g(int i, int j, ppm *colr, kfil *kern, int aux)
{
	dbl e = 0;
	dbl s = 0;
	dbl b = 0;
	dbl g = 0;
	for (int k = i - 1; k < i + 2; ++k) {
		for (int q = j - 1; q < j + 2; ++q) {
			e += colr->pix[k][q].g * kern->edge[k - (i - 1)][q - (j - 1)];
			s += colr->pix[k][q].g * kern->shar[k - (i - 1)][q - (j - 1)];
			b += colr->pix[k][q].g * kern->blur[k - (i - 1)][q - (j - 1)];
			g += colr->pix[k][q].g * kern->gsblur[k - (i - 1)][q - (j - 1)];
		}
	}
	if (aux == 1) {
		e /= 10000;
		e = rr(e);
		return e;
	}
	if (aux == 2) {
		s /= 10000;
		s = rr(s);
		return s;
	}
	if (aux == 3) {
		b /= 10000;
		b = rr(b);
		return b;
	}
	if (aux == 4) {
		g /= 10000;
		g = rr(g);
		return g;
	}
	return 10000;
}

//operations for blue pixels
dbl final_pixel_b(int i, int j, ppm *colr, kfil *kern, int aux)
{
	dbl e = 0;
	dbl s = 0;
	dbl b = 0;
	dbl g = 0;
	for (int k = i - 1; k < i + 2; ++k) {
		for (int q = j - 1; q < j + 2; ++q) {
			e += colr->pix[k][q].b * kern->edge[k - (i - 1)][q - (j - 1)];
			s += colr->pix[k][q].b * kern->shar[k - (i - 1)][q - (j - 1)];
			b += colr->pix[k][q].b * kern->blur[k - (i - 1)][q - (j - 1)];
			g += colr->pix[k][q].b * kern->gsblur[k - (i - 1)][q - (j - 1)];
		}
	}
	if (aux == 1) {
		e /= 10000;
		e = rr(e);
		return e;
	}
	if (aux == 2) {
		s /= 10000;
		s = rr(s);
		return s;
	}
	if (aux == 3) {
		b /= 10000;
		b = rr(b);
		return b;
	}
	if (aux == 4) {
		g /= 10000;
		g = rr(g);
		return g;
	}
	return 1000;
}

//initialise the matrix
//for red, green and blue
void put_them_work(dbl **mr, dbl **mg, dbl **mb, ppm *colr, int lin, int col)
{
	for (int i = 0; i < lin; ++i) {
		for (int j = 0; j < col; ++j) {
			mr[i][j] = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].r;
			mg[i][j] = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].g;
			mb[i][j] = colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].b;
		}
	}
}

//time_to_modify(mr, mg, mb, colr, lin, col);
void time_to_modify(dbl **mr, dbl **mg, dbl **mb, ppm *colr, int l, int c)
{
	for (int i = 0; i < l; ++i) {
		for (int j = 0; j < c; ++j) {
			colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].r = mr[i][j];
			colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].g = mg[i][j];
			colr->pix[i + colr->y_c[2]][j + colr->x_c[2]].b = mb[i][j];
		}
	}
}

//decide which filter should I apply
void decide_fil(ppm *colr, kfil *kern, int aux)
{
	int col = colr->x_c[3] - colr->x_c[2];
	int lin = colr->y_c[3] - colr->y_c[2];
	dbl **mr;
	dbl **mg;
	dbl **mb;
	mr = malloc(lin * sizeof(dbl *));
	if (!mr) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	for (int i = 0; i < lin; ++i) {
		mr[i] = malloc(col * sizeof(dbl));
		if (!mr[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	mg = malloc(lin * sizeof(dbl *));
	if (!mg) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	for (int i = 0; i < lin; ++i) {
		mg[i] = malloc(col * sizeof(dbl));
		if (!mg[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	mb = malloc(lin * sizeof(dbl *));
	if (!mb) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}
	for (int i = 0; i < lin; ++i) {
		mb[i] = malloc(col * sizeof(dbl));
		if (!mb[i]) {
			fprintf(stderr, "Malloc failed\n");
			return;
		}
	}
	put_them_work(mr, mg, mb, colr, lin, col);
	for (int i = 0; i < lin; ++i) {
		for (int j = 0; j < col; ++j) {
			//h is used to provide the information if the pixels can be
			//modified
			int h = cond(i + colr->y_c[2], j + colr->x_c[2], colr);
			if (h == 0) {
				int k = i + colr->y_c[2];
				int q = j + colr->x_c[2];
				mg[i][j] = final_pixel_g(k, q, colr, kern, aux);
				mr[i][j] = final_pixel_red(k, q, colr, kern, aux);
				mb[i][j] = final_pixel_b(k, q, colr, kern, aux);
			}
		}
	}
	time_to_modify(mr, mg, mb, colr, lin, col);
	for (int i = 0; i < lin; ++i) {
		free(mr[i]);
		free(mg[i]);
		free(mb[i]);
	}
	free(mr);
	free(mg);
	free(mb);
}

void filters(char *com, char *buf, pgm *grey, ppm *colr, kfil *kern)
{
	buf = strtok(NULL, " ");
	int what_filter = is_okayp(com, buf);
	//conditions to make the filter happen
	if (colr->mood == 0 && grey->mood == 0) {
		printf("No image loaded\n");
		return;
	}
	if (what_filter == -1) {
		printf("Invalid command\n");
		return;
	}
	if (grey->mood == 1) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	if (what_filter == 0) {
		printf("APPLY parameter invalid\n");
		return;
	}
	if (what_filter > 0 && colr->mood == 1) {
		decide_fil(colr, kern, what_filter);
		printf("APPLY %s done\n", buf);
		return;
	}
}

void init_filt_par(kfil *kern)
{
	//edge init
	kern->edge[0][0] = -10000;
	kern->edge[0][1] = -10000;
	kern->edge[0][2] = -10000;
	kern->edge[1][0] = -10000;
	kern->edge[1][1] = 80000;
	kern->edge[1][2] = -10000;
	kern->edge[2][0] = -10000;
	kern->edge[2][1] = -10000;
	kern->edge[2][2] = -10000;
	//sharpen init
	kern->shar[0][0] = 0;
	kern->shar[0][1] = -10000;
	kern->shar[0][2] = 0;
	kern->shar[1][0] = -10000;
	kern->shar[1][1] = 50000;
	kern->shar[1][2] = -10000;
	kern->shar[2][0] = 0;
	kern->shar[2][1] = -10000;
	kern->shar[2][2] = 0;
	//blur init
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j)
			kern->blur[i][j] = 1111;
	}
	//gaussian blur init
	kern->gsblur[0][0] = 625;
	kern->gsblur[0][1] = 1250;
	kern->gsblur[0][2] = 625;
	kern->gsblur[1][0] = 1250;
	kern->gsblur[1][1] = 2500;
	kern->gsblur[1][2] = 1250;
	kern->gsblur[2][0] = 625;
	kern->gsblur[2][1] = 1250;
	kern->gsblur[2][2] = 625;
	/*for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			printf("%f ", kern->edge[i][j]);
		}
		printf("\n");
	}*/
}

void exittt(ppm *colr, pgm *grey)
{
	//free the memory
	if (colr->mood == 0 && grey->mood == 0) {
		printf("No image loaded\n");
		return;
	}
	free_that(grey, colr);
}

int main(void)
{
	char *command, *buffer;
	pgm grey;
	ppm colr;
	kfil kern;
	//initialise the fact that the image is not loaded yet
	//because the program hasn't started the loop
	init_filt_par(&kern);
	colr.mood = 0;
	grey.mood = 0;
	alloc_dynamic_char(&command, &buffer);
	FILE *image;
	image = NULL;
	int k = 0;
	while (1 != 0) {
		if (k == 1)
			break;
		read_and_select(command, buffer);
		if (strcmp(buffer, "LOAD") == 0) {
			load_image(image, command, buffer, &grey, &colr);
		} else if (strcmp(buffer, "SELECT") == 0) {
			selection_op(command, buffer, &grey, &colr);
		} else if (strcmp(buffer, "ROTATE") == 0) {
			rotate_op(command, buffer, &grey, &colr);
		} else if (strcmp(buffer, "CROP") == 0) {
			crop_op(&grey, &colr);
		} else if (strcmp(buffer, "SAVE") == 0) {
			save(command, buffer, &grey, &colr, image);
		} else if (strcmp(buffer, "APPLY") == 0) {
			filters(command, buffer, &grey, &colr, &kern);
		} else if (strcmp(buffer, "EXIT") == 0) {
			exittt(&colr, &grey);
			k = 1;
		} else {
			printf("Invalid command\n");
		}
	}
	free(command);
	free(buffer);
	return 0;
}
