#include "matrix.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// Vector Operations
// ============================================================================

Vector *vector_create(size_t size) {
    assert(size > 0 && "Vector size must be positive");
    
    Vector *v = malloc(sizeof(Vector));
    assert(v != NULL && "Failed to allocate Vector structure");
    
    v->size = size;
    v->data = malloc(size * sizeof(double));
    assert(v->data != NULL && "Failed to allocate Vector data");
    
    // Initialize all elements to 0.0
    for (size_t i = 0; i < size; i++) {
        v->data[i] = 0.0;
    }
    
    return v;
}

void vector_free(Vector *v) {
    if (v == NULL) return;
    
    free(v->data);
    free(v);
}

void vector_set(Vector *v, size_t index, double value) {
    assert(v != NULL && "Vector cannot be NULL");
    assert(index < v->size && "Index out of bounds");
    
    v->data[index] = value;
}

double vector_get(const Vector *v, size_t index) {
    assert(v != NULL && "Vector cannot be NULL");
    assert(index < v->size && "Index out of bounds");
    
    return v->data[index];
}

void vector_print(const Vector *v) {
    assert(v != NULL && "Vector cannot be NULL");
    
    printf("[");
    for (size_t i = 0; i < v->size; i++) {
        printf("%g", v->data[i]);
        if (i < v->size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

// ============================================================================
// Matrix Operations
// ============================================================================

Matrix *matrix_create(size_t rows, size_t cols) {
    assert(rows > 0 && "Matrix rows must be positive");
    assert(cols > 0 && "Matrix cols must be positive");
    
    Matrix *m = malloc(sizeof(Matrix));
    assert(m != NULL && "Failed to allocate Matrix structure");
    
    m->rows = rows;
    m->cols = cols;
    
    // Allocate array of row pointers
    m->data = malloc(rows * sizeof(double *));
    assert(m->data != NULL && "Failed to allocate Matrix row pointers");
    
    // Allocate each row
    for (size_t i = 0; i < rows; i++) {
        m->data[i] = malloc(cols * sizeof(double));
        assert(m->data[i] != NULL && "Failed to allocate Matrix row");
        
        // Initialize all elements to 0.0
        for (size_t j = 0; j < cols; j++) {
            m->data[i][j] = 0.0;
        }
    }
    
    return m;
}

void matrix_free(Matrix *m) {
    if (m == NULL) return;
    
    // Free each row
    for (size_t i = 0; i < m->rows; i++) {
        free(m->data[i]);
    }
    
    // Free row pointer array
    free(m->data);
    free(m);
}

void matrix_set(Matrix *m, size_t row, size_t col, double value) {
    assert(m != NULL && "Matrix cannot be NULL");
    assert(row < m->rows && "Row index out of bounds");
    assert(col < m->cols && "Column index out of bounds");
    
    m->data[row][col] = value;
}

double matrix_get(const Matrix *m, size_t row, size_t col) {
    assert(m != NULL && "Matrix cannot be NULL");
    assert(row < m->rows && "Row index out of bounds");
    assert(col < m->cols && "Column index out of bounds");
    
    return m->data[row][col];
}

void matrix_print(const Matrix *m) {
    assert(m != NULL && "Matrix cannot be NULL");
    
    for (size_t i = 0; i < m->rows; i++) {
        printf("[");
        for (size_t j = 0; j < m->cols; j++) {
            printf("%g", m->data[i][j]);
            if (j < m->cols - 1) {
                printf("  ");
            }
        }
        printf("]\n");
    }
}

// ============================================================================
// Matrix-Vector Operations
// ============================================================================

Vector *matrix_multiply_vector(const Matrix *m, const Vector *v) {
    assert(m != NULL && "Matrix cannot be NULL");
    assert(v != NULL && "Vector cannot be NULL");
    assert(m->cols == v->size && "Matrix columns must equal vector size");
    
    Vector *result = vector_create(m->rows);
    
    // For each row in the matrix
    for (size_t i = 0; i < m->rows; i++) {
        double sum = 0.0;
        
        // Dot product of row i with the vector
        for (size_t j = 0; j < m->cols; j++) {
            sum += m->data[i][j] * v->data[j];
        }
        
        result->data[i] = sum;
    }
    
    return result;
}
