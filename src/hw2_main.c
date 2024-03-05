#include "hw2.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>


typedef struct Pixel {
	unsigned char r, g, b;
} Pixel;


typedef struct Image {
	int width, height;
	Pixel **pixels;
} Image;


typedef struct CopyParams {
	int row, column, width, height;
} CopyParams;


typedef struct PasteParams {
	int row, column;
} PasteParams;


typedef struct RenderParams {
	char *message;
	char *fontPath;
	int fontSize;
	int row;
	int col;
} RenderParams;


typedef struct FontChar {
	char key; 
	char **data;
	int rows; 
	int cols;
} FontChar;


typedef struct Font {
	FontChar *characters;   
	int numChars; 
} Font;


bool charIn(int option, const char *string);



bool startWith(const char *filename, const char *string);


bool checkCopyParams(char *params);


char **split(const char *str, const char *delim);


int count_words(const char *str, const char *delim, int numwords);


int arrayLength(char **pString);


void freeArr(char **pString);


bool checkPasteParams(char *params);


bool checkRenderParams(char *params);


const char *getExt(const char *filename);


Image load_image(const char *filepath);


Image load_ppm(const char *filepath);


Image load_sbu(const char *filepath);


void save_image(const Image *img, const char *filepath);


void save_as_ppm(const Image *img, const char *filepath);


void save_as_sbu(const Image *img, const char *filepath);


Pixel **allocate_pixels(int width, int height);


void free_pixels(Pixel **pixels, int height);


void free_image(Image img);


bool pixelArrayContainsColor(Pixel *table, Pixel param, int size);


int getPosition(Pixel *table, Pixel pixel, int colors);


int copy_paste(Image *ptr, CopyParams copy, PasteParams paste);


int print_message(Image *ptr, RenderParams render);


char **loadFontsRaw(const char *filename);


FontChar *getFontChar(Font *font, char key);


void printFontRaw(char **fonts);


void freeFontsRaw(char **fonts);


void initFont(Font *font, int numChars);


void addFontChar(Font *font, char key, char **data, int rows, int cols, int index);


void freeFont(Font *font);


Font *loadFont(char **rawFont);


int scaleFont(Font *font, int fontSize);


bool isEmptyCol(char **pString, size_t j);


void freeFontChar(const Font *font, int i);


bool fontOverlaps(Image *ptr, int row, int col, FontChar *pChar);

int main(int argc, char **argv) {
	if (argc < 2) {
		return MISSING_ARGUMENT;
	}

	int o;
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	int flag4 = 0;
	int flag5 = 0;
	char *input_filename = NULL, *output_filename = NULL;
	char *copyParams = NULL;
	char *pasteParams = NULL;
	char *renderParams = NULL;
	while ((o = getopt(argc, argv, "i:o:c:p:r:")) != -1) {
		switch (o) {
			case 'i':
				if (flag1) return DUPLICATE_ARGUMENT;
				flag1 = true;
				input_filename = optarg;
				if (input_filename == NULL) return MISSING_ARGUMENT;
				if (startWith(input_filename, "-")) return MISSING_ARGUMENT;
				break;
			case 'o':
				if (flag2) return DUPLICATE_ARGUMENT;
				flag2 = true;
				output_filename = optarg;
				if (startWith(output_filename, "-")) return MISSING_ARGUMENT;
				break;
			case 'c':
				if (flag3) return DUPLICATE_ARGUMENT;
				flag3 = 1;
				copyParams = optarg;
				if (startWith(copyParams, "-")) return MISSING_ARGUMENT;
				break;
			case 'p':
				if (flag4) return DUPLICATE_ARGUMENT;
				flag4 = 1;
				pasteParams = optarg;
				if (startWith(pasteParams, "-")) return MISSING_ARGUMENT;
				break;
			case 'r':
				if (flag5) return DUPLICATE_ARGUMENT;
				flag5 = 1;
				renderParams = optarg;
				if (startWith(renderParams, "-")) return MISSING_ARGUMENT;
				break;
			default:
				if (charIn(optopt, "iocpr") == false) {
					return UNRECOGNIZED_ARGUMENT;
				} else {
					return MISSING_ARGUMENT;
				}
		}
	}

	if (!flag1 || !flag2) return MISSING_ARGUMENT;
	if (access(input_filename, F_OK) == -1) return INPUT_FILE_MISSING;
	FILE *file = fopen(output_filename, "w");
	if (file == NULL) return OUTPUT_FILE_UNWRITABLE;
	fclose(file);
	if (flag4 && !flag3) return C_ARGUMENT_MISSING;
	if (flag3 && checkCopyParams(copyParams) == false) return C_ARGUMENT_INVALID;
	if (flag4 && checkPasteParams(pasteParams) == false) return P_ARGUMENT_INVALID;
	if (flag5 && checkRenderParams(renderParams) == false) return R_ARGUMENT_INVALID;

	Image img = load_image(input_filename);

	if (flag3) {
		char **pString = split(copyParams, ",");
		CopyParams copy = {atoi(pString[0]), atoi(pString[1]), atoi(pString[2]), atoi(pString[3])};
		freeArr(pString);

		if (flag4) {
			pString = split(pasteParams, ",");
			PasteParams paste = {atoi(pString[0]), atoi(pString[1])};
			freeArr(pString);

			copy_paste(&img, copy, paste);
		}

	}

	if (flag5) {
		char **pString = split(renderParams, ",");

		RenderParams render = {pString[0], pString[1], atoi(pString[2]), atoi(pString[3]), atoi(pString[4])};
		int result = print_message(&img, render);
		if (result != 0) {
			freeArr(pString);
			free_image(img);
			return result;
		}
		freeArr(pString);

	}
	save_image(&img, output_filename);

	free_image(img);
	(void) argc;
	(void) argv;

	return 0;
}

int copy_paste(Image *ptr, CopyParams copy, PasteParams paste) {
	int srcRow = copy.row;
	int srcCol = copy.column;
	int destRow = paste.row;
	int destCol = paste.column;
	int width = copy.width;
	int height = copy.height;

	Pixel **temp = allocate_pixels(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (srcRow + i >= ptr->height || srcCol + j >= ptr->width) {
				continue;
			}
			temp[i][j] = ptr->pixels[srcRow + i][srcCol + j];
		}
	}

	for (int i = 0; i < height; i++) {
		if (destRow + i >= ptr->height || srcRow + i >= ptr->height) {
			continue;
		}
		for (int j = 0; j < width; j++) {
			if (destCol + j >= ptr->width || srcCol + j >= ptr->width) {
				continue;
			}
			ptr->pixels[destRow + i][destCol + j] = temp[i][j];
		}
	}

	free_pixels(temp, height);

	return 0;
}

bool checkRenderParams(char *params) {
	char **pString = split(params, ",");
	if (pString == NULL) {
		return false;
	}
	if (arrayLength(pString) != 5) {
		freeArr(pString);
		return false;
	}

	char *path = pString[1];
	if (access(path, F_OK) == -1) {
		freeArr(pString);
		return false;
	}

	for (int i = 2; i < 5; i++) {
		for (int j = 0; pString[i][j]; j++) {
			if (!isdigit(pString[i][j])) {
				freeArr(pString);
				return false;
			}
		}
	}
	freeArr(pString);
	return true;
}

bool checkPasteParams(char *params) {
	char **pString = split(params, ",");
	if (pString == NULL) {
		return false;
	}

	if (arrayLength(pString) != 2) {
		freeArr(pString);
		return false;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; pString[i][j]; j++) {
			if (!isdigit(pString[i][j])) {
				freeArr(pString);
				return false;
			}
		}
	}
	freeArr(pString);
	return true;
}

int count_words(const char *str, const char *delim, int numwords) {
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		if (!charIn(str[i], delim) &&
			(charIn(str[i + 1], delim) || !str[i + 1])) {
			numwords++;
		}
	}
	return (numwords);
}

char **split(const char *str, const char *delim) {
	int i, j, k, m, numwords = 0;
	char **s;

	if (str == NULL || str[0] == 0)
		return (NULL);
	if (!delim)
		delim = " ";

	numwords = count_words(str, delim, numwords);
	if (numwords == 0) {
		return (NULL);
	}
	s = malloc((1 + numwords) * sizeof(char *));
	if (!s)
		return (NULL);
	for (i = 0, j = 0; j < numwords; j++) {
		while (charIn(str[i], delim) == 1)
			i++;

		k = 0;
		while (!charIn(str[i + k], delim) && str[i + k])
			k++;

		s[j] = malloc((k + 1) * sizeof(char));
		if (!s[j]) {
			for (k = 0; k < j; k++)
				free(s[k]);

			free(s);
			return (NULL);
		}
		for (m = 0; m < k; m++)
			s[j][m] = str[i++];
		s[j][m] = 0;
	}
	s[j] = NULL;
	return (s);
}

int arrayLength(char **pString) {
	int length = 0;
	while (pString[length] != NULL) {
		length++;
	}
	return length;
}

void freeArr(char **pString) {
	for (int i = 0; pString[i] != NULL; i++) {
		free(pString[i]);
	}
	free(pString);
}

bool checkCopyParams(char *params) {
	char **pString = split(params, ",");
	if (pString == NULL) {
		return false;
	}
	if (arrayLength(pString) != 4) {
		freeArr(pString);
		return false;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; pString[i][j]; j++) {
			if (!isdigit(pString[i][j])) {
				freeArr(pString);
				return false;
			}
		}
	}
	freeArr(pString);
	return true;
}

bool startWith(const char *string1, const char *string) {
	for (size_t i = 0; i < strlen(string); i++) {
		if (string1[i] != string[i]) {
			return false;
		}
	}
	return true;
}

bool charIn(int option, const char *string) {
	for (size_t i = 0; i < strlen(string); i++) {
		if (option == string[i]) {
			return true;
		}
	}
	return false;
}

const char *getExt(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return ""; 
	return dot + 1; 
}

Pixel **allocate_pixels(int width, int height) {
	Pixel **pixels = malloc(height * sizeof(Pixel *));
	for (int i = 0; i < height; i++) {
		pixels[i] = malloc(width * sizeof(Pixel));
		pixels[i] = memset(pixels[i], 0, width * sizeof(Pixel));


	}
	return pixels;
}

void free_pixels(Pixel **pixels, int height) {
	for (int i = 0; i < height; i++) {
		free(pixels[i]);
	}
	free(pixels);
}

void free_image(Image img) {
	if (img.pixels == NULL) return;
	free_pixels(img.pixels, img.height);
}

Image load_image(const char *filepath) {
	const char *extension = getExt(filepath);
	Image img;

	if (strcmp(extension, "ppm") == 0) {
		img = load_ppm(filepath);
	} else if (strcmp(extension, "sbu") == 0) {
		img = load_sbu(filepath);
	} else {
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
	}

	return img;
}

Image load_ppm(const char *filepath) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		return img;
	}

	char magic[3];
	fscanf(file, "%s", magic);
	if (strcmp(magic, "P3") != 0) {
		Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		fclose(file);
		return img;
	}

	int width, height, max;
	fscanf(file, "%d %d %d", &width, &height, &max);
	Pixel **pixels = allocate_pixels(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fscanf(file, "%hhu %hhu %hhu", &pixels[i][j].r, &pixels[i][j].g, &pixels[i][j].b);
		}
	}
	fclose(file);

	Image img;
	img.width = width;
	img.height = height;
	img.pixels = pixels;
	return img;
}

void save_image(const Image *img, const char *filepath) {
	const char *extension = getExt(filepath);

	if (strcmp(extension, "ppm") == 0) {
		save_as_ppm(img, filepath);
	} else if (strcmp(extension, "sbu") == 0) {
		save_as_sbu(img, filepath);
	} else {
		perror("Unsupported file type");
		exit(EXIT_FAILURE);
	}
}

void save_as_ppm(const Image *img, const char *filepath) {
	FILE *file = fopen(filepath, "w");
	if (!file) {
		
		return;
	}

	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", img->width, img->height);
	fprintf(file, "255\n");
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			fprintf(file, "%hhu %hhu %hhu ", img->pixels[i][j].r, img->pixels[i][j].g, img->pixels[i][j].b);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

Image load_sbu(const char *filepath) {
	FILE *file = fopen(filepath, "r");
	if (!file) {

	Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		return img;
	}

	char magic[4];
	fscanf(file, "%s", magic);
	if (strcmp(magic, "SBU") != 0) {
		Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		fclose(file);
		return img;
	}

	int width, height;
	fscanf(file, "%d %d", &width, &height);

	int num_colors;
	fscanf(file, "%d", &num_colors);

	Pixel color_table[num_colors];
	for (int i = 0; i < num_colors; i++) {
		fscanf(file, "%hhu %hhu %hhu", &color_table[i].r, &color_table[i].g, &color_table[i].b);
	}

	Pixel **pixels = allocate_pixels(width, height);


	int i = 0;
	int j = 0;
	int color_index;
	int run_length;
	char c;

	while ((c = fgetc(file)) != EOF) {
		if (isdigit(c)) {
			ungetc(c, file);
			fscanf(file, "%d", &color_index);
			pixels[i][j] = color_table[color_index];
			j++;
			if (j == width) {
				j = 0;
				i++;
			}
		} else if (c == '*') {
			fscanf(file, "%d %d", &run_length, &color_index);

			for (int k = 0; k < run_length; k++) {
				pixels[i][j] = color_table[color_index];
				j++;
				if (j == width) {
					j = 0;
					i++;
				}

				if (i == height) {
					break;
				}
			}
		}
	}

	fclose(file);

	Image img;
	img.width = width;
	img.height = height;
	img.pixels = pixels;
	return img;
}

void save_as_sbu(const Image *img, const char *filepath) {
	FILE *file = fopen(filepath, "w");
	if (!file) {
		return;
	}

	fprintf(file, "SBU\n");
	fprintf(file, "%d %d\n", img->width, img->height);
	int num_colors = 0;
	Pixel *color_table = malloc(1 * sizeof(Pixel));
	Pixel *temp = NULL;
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			if (!pixelArrayContainsColor(color_table, img->pixels[i][j], num_colors)) {
				num_colors++;
				temp = reallocarray(color_table, num_colors, sizeof(Pixel));
				if (temp == NULL) {
					free(color_table);
					fprintf(stderr, "Failed to allocate memory.\n");
					exit(EXIT_FAILURE);
				}
				color_table = temp;
				color_table[num_colors - 1] = img->pixels[i][j];
			}
		}
	}

	fprintf(file, "%d ", num_colors);
	for (int i = 0; i < num_colors; i++) {
		fprintf(file, "%hhu %hhu %hhu ", color_table[i].r, color_table[i].g, color_table[i].b);
	}

	fprintf(file, "\n");
	int run_length;
	int j = 0;
	int i = 0;
	while (i < img->height) {
		run_length = 1;
		Pixel current_pixel = img->pixels[i][j];
		while (true) {
			j++;
			if (j == img->width) {
				j = 0;
				i++;
			}
			if (i == img->height) {
				break;
			}
			if (img->pixels[i][j].r == current_pixel.r && img->pixels[i][j].g == current_pixel.g &&
				img->pixels[i][j].b == current_pixel.b) {
				run_length++;
			} else {
				break;
			}
		}


		int pos = getPosition(color_table, current_pixel, num_colors);
		if (pos == -1) {
			fprintf(stderr, "Failed to find color in color table.\n");
			exit(EXIT_FAILURE);
		}
		if (run_length == 1) {
			fprintf(file, "%d ", pos);
		} else {
			fprintf(file, "*%d %d ", run_length, pos);
		}

	}

	free(color_table);
	color_table = NULL;
	fclose(file);
}

int getPosition(Pixel *table, Pixel pixel, int colors) {
	for (int i = 0; i < colors; i++) {
		if (table[i].r == pixel.r && table[i].g == pixel.g && table[i].b == pixel.b) {
			return i;
		}
	}
	return -1;
}

bool pixelArrayContainsColor(Pixel *table, Pixel param, int size) {
	for (int i = 0; i < size; i++) {
		if (table[i].r == param.r && table[i].g == param.g && table[i].b == param.b) {
			return true;
		}
	}
	return false;
}

char **loadFontsRaw(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		return NULL;
	}

	char **fonts = NULL;
	size_t rowsAllocated = 0;
	size_t currentRowLength = 0;
	size_t rows = 0;
	int ch;
	char *lineBuffer = NULL;
	size_t lineBufferSize = 0;

	while ((ch = fgetc(file)) != EOF) {
		if (ch == '\n' || ch == '\r') {
			if (rows == rowsAllocated) {
				size_t newSize = rowsAllocated == 0 ? 1 : rowsAllocated * 2;
				char **newFonts = realloc(fonts, newSize * sizeof(char *));
				if (!newFonts) {
					fclose(file);
					return NULL;
				}

				if (newSize > rowsAllocated) {
					for (size_t i = rowsAllocated; i < newSize; i++) {
						newFonts[i] = NULL;
					}
				}

				fonts = newFonts;
				rowsAllocated = newSize;
			}
			lineBuffer = realloc(lineBuffer, currentRowLength + 1);
			lineBuffer[currentRowLength] = '\0';
			fonts[rows++] = lineBuffer;
			lineBuffer = NULL;
			lineBufferSize = 0;
			currentRowLength = 0;
		} else {
			if (currentRowLength == lineBufferSize) {
				size_t newSize = lineBufferSize == 0 ? 1 : lineBufferSize * 2;
				char *newLineBuffer = realloc(lineBuffer, newSize);
				if (!newLineBuffer) {
					fclose(file);
					return NULL;
				}
				lineBuffer = newLineBuffer;
				lineBufferSize = newSize;
			}
			lineBuffer[currentRowLength++] = (char) ch;
		}
	}
	if (currentRowLength > 0) {
		lineBuffer = realloc(lineBuffer, currentRowLength + 1);
		lineBuffer[currentRowLength] = '\0';
		if (rows == rowsAllocated) {
			char **newFonts = realloc(fonts, (rowsAllocated + 1) * sizeof(char *));
			if (!newFonts) {
				fclose(file);
				return NULL;
			}
			fonts = newFonts;
		}
		fonts[rows++] = lineBuffer;
	} 
	else if (lineBuffer != NULL) {
		free(lineBuffer);
	}

	fclose(file);
	return fonts;
}

void printFontRaw(char **fonts) {
	int rows = 0;
	int cols = 0;
	for (size_t i = 0; fonts[i] != NULL; i++) {
		char *row = fonts[i];
		cols = 0;
		for (size_t j = 0; row[j] != '\0'; j++) {
			if (isEmptyCol(fonts, j)) {
				printf("%s", "  ");
			} else {
				printf("%c", row[j]);
			}

			cols++;
		}
		printf("\n");
		rows++;
	}

	printf("Number of rows: %d\n", rows);
	printf("Number of cols: %d\n", cols);
}

bool isEmptyCol(char **pString, size_t j) {
	for (size_t i = 0; pString[i] != NULL; i++) {
		char character = pString[i][j];
		if (character != ' ') {
			return false;
		}
	}
	return true;
}

void freeFontsRaw(char **fonts) {
	if (!fonts) {
		return;
	}

	for (int i = 0; fonts[i] != NULL; i++) {
		free(fonts[i]);
	}

	free(fonts);
}

void initFont(Font *font, int numChars) {
	font->numChars = numChars;
	font->characters = (FontChar *) malloc(numChars * sizeof(FontChar));
}

void addFontChar(Font *font, char key, char **data, int rows, int cols, int index) {
	if (index >= font->numChars) return;

	font->characters[index].key = key;
	font->characters[index].rows = rows;
	font->characters[index].cols = cols;
	font->characters[index].data = data;

}

void freeFont(Font *font) {
	for (int i = 0; i < font->numChars; i++) {
		freeFontChar(font, i);
	}
	free(font->characters);
}

void freeFontChar(const Font *font, int i) {
	for (int j = 0; j < font->characters[i].rows; j++) {
		free(font->characters[i].data[j]);
	}
	free(font->characters[i].data);
}

Font *loadFont(char **rawFont) {

	int numChars = 0;
	int rows = 0;
	for (int i = 0; rawFont[i] != NULL; i++) {
		char *row = rawFont[i];
		numChars = 0;
		for (size_t j = 0; row[j] != '\0'; j++) {
			if (j == 0) {
				while (isEmptyCol(rawFont, j)) {
					j++;
				}
			}

			if (isEmptyCol(rawFont, j)) {
				while (isEmptyCol(rawFont, (j + 1))) {
					j++;
				}
				numChars++;
			}

		}
		rows++;
	}

	Font *font = (Font *) malloc(sizeof(Font));
	initFont(font, numChars);

	int startCol = 0;
	char *keys = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < numChars; i++) {
		char key = keys[i];

		if (i == 0) {
			while (isEmptyCol(rawFont, startCol)) {
				startCol++;
			}
		}


		int endCol = startCol;
		while (isEmptyCol(rawFont, endCol) == 0) {
			endCol++;
		}
		int cols = endCol - startCol;
		char **data = (char **) malloc(rows * sizeof(char *));
		for (int j = 0; j < rows; j++) {
			data[j] = (char *) malloc((cols + 1) * sizeof(char));
			strncpy(data[j], rawFont[j] + startCol, cols);
			data[j][cols] = '\0';
		}
		addFontChar(font, key, data, rows, cols, i);
		startCol = endCol + 1;

		while (isEmptyCol(rawFont, startCol)) {
			startCol++;
		}

	}

	return font;
}

FontChar *getFontChar(Font *font, char key) {
	key = (char) toupper((int) key);
	for (int i = 0; i < font->numChars; i++) {
		if (font->characters[i].key == key) {
			return &font->characters[i];
		}
	}
	return NULL;
}

void printFontChar(FontChar *fontChar) {
	printf("Character: %c\n", fontChar->key);
	printf("Rows: %d\n", fontChar->rows);
	printf("Cols: %d\n", fontChar->cols);
	for (int i = 0; i < fontChar->rows; i++) {
		printf("%s\n", fontChar->data[i]);
	}
}

int scaleFont(Font *font, int fontSize) {
	for (int i = 0; i < font->numChars; i++) {
		FontChar *fontChar = &font->characters[i];
		int rows = fontChar->rows;
		int cols = fontChar->cols;
		char **data = (char **) malloc(fontSize * rows * sizeof(char *));
		memset(data, 0, fontSize * rows * sizeof(char *));
		for (int j = 0; j < fontSize * rows; j++) {
			data[j] = (char *) malloc((1 + (fontSize * cols)) * sizeof(char));
			memset(data[j], 0, (1 + (fontSize * cols)) * sizeof(char));
		}


		for (int j = 0; j < rows; j++) {
			for (int k = 0; k < cols; k++) {
				for (int l = 0; l < fontSize; l++) {
					for (int m = 0; m < fontSize; m++) {
						data[j * fontSize + l][k * fontSize + m] = fontChar->data[j][k];
					}
				}
			}
		}

		freeFontChar(font, i);

		fontChar->data = data;
		fontChar->rows = fontSize * rows;
		fontChar->cols = fontSize * cols;
	}
	return 0;
}

int print_message(Image *ptr, RenderParams render) {
	char *fontsPath = render.fontPath;
	char **fonts = loadFontsRaw(fontsPath);
	int fontSize = render.fontSize;
	char *message = render.message;
	if (fonts == NULL) {
		(void) ptr;
		return MISSING_ARGUMENT;
	}

	Font *font = loadFont(fonts);
	scaleFont(font, fontSize);


	for (size_t i = 0; i < strlen(message); i++) {
		if (message[i] == ' ') {
			render.col += 5;
			continue;
		}

		FontChar *fontChar = getFontChar(font, message[i]);

		if (fontChar == NULL) {
			freeFontsRaw(fonts);
			freeFont(font);
			free(font);
			(void) ptr;
			return MISSING_ARGUMENT;
		}

		if (!fontOverlaps(ptr, render.row, render.col, fontChar)) {
			for (int j = 0; j < fontChar->rows; j++) {
				for (int k = 0; k < fontChar->cols; k++) {
					if (fontChar->data[j][k] == '*') {
						if (render.row + j >= ptr->height || render.col + k >= ptr->width) {
							continue;
						}
						ptr->pixels[render.row + j][render.col + k].r = 255;
						ptr->pixels[render.row + j][render.col + k].g = 255;
						ptr->pixels[render.row + j][render.col + k].b = 255;
					}
				}
			}
		}
		render.col += fontChar->cols + 1;
	}


	freeFontsRaw(fonts);
	freeFont(font);
	free(font);
	(void) ptr;
	return 0;
}

bool fontOverlaps(Image *ptr, int row, int col, FontChar *pChar) {
	for (int i = 0; i < pChar->rows; i++) {
		for (int j = 0; j < pChar->cols; j++) {
			if (pChar->data[i][j] == '*' && (row + i >= ptr->height || col + j >= ptr->width)) {
				return true;
			}
		}
	}
	return false;
}

