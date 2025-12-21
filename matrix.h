#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct {
    double *data;
    size_t size;
} Vector;

typedef struct {
    double **data;
    size_t rows;
    size_t cols;
} Matrix;

// Vector operations
Vector *vector_create(size_t size);
void vector_free(Vector *v);
void vector_set(Vector *v, size_t index, double value);
double vector_get(const Vector *v, size_t index);
void vector_print(const Vector *v);

// Matrix operations
Matrix *matrix_create(size_t rows, size_t cols);
void matrix_free(Matrix *m);
void matrix_set(Matrix *m, size_t row, size_t col, double value);
double matrix_get(const Matrix *m, size_t row, size_t col);
void matrix_print(const Matrix *m);

// Matrix-vector multiplication
Vector *matrix_multiply_vector(const Matrix *m, const Vector *v);

#endif // MATRIX_H
