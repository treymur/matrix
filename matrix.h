#pragma once
#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
// #include <initializer_list>  /* included in <vector> */

#define ULL_int unsigned long long int

class Matrix {
public:
    /* Constructors */

    Matrix();
    Matrix(std::initializer_list<std::initializer_list<double>> in);
    Matrix(std::vector<std::vector<double>> in);
    Matrix(std::vector<std::vector<int>> in);
    Matrix(const Matrix& other);
    Matrix(ULL_int rows, ULL_int columns, double value);
    Matrix(ULL_int rows, ULL_int columns);
    Matrix(ULL_int identitySize);
    
    /* Get functions */

    int num_rows() const;
    int num_columns() const;
    std::vector<double> get_row(ULL_int row) const;
    std::vector<double> get_column(ULL_int col) const;
    double& at(ULL_int row, ULL_int col);
    int size() const;
    bool empty() const;

    /* Edit functions */

    void push_back_row(const std::initializer_list<double>& row);
    void push_back_row(const std::vector<double>& row);
    void push_back_row(const std::vector<int>& row);
    void push_back_row(double value);
    void push_back_row();

    void push_back_column(const std::initializer_list<double>& col);
    void push_back_column(const std::vector<double>& col);
    void push_back_column(const std::vector<int>& col);
    void push_back_column(double value);
    void push_back_column();

    void set_row(ULL_int row, const std::initializer_list<double>& rowNew);
    void set_row(ULL_int row, const std::vector<double>& rowNew);
    void set_row(ULL_int row, const std::vector<int>& rowNew);
    void set_row(ULL_int row, double value);
    void set_row(ULL_int row);

    void set_column(ULL_int col, const std::initializer_list<double>& colNew);
    void set_column(ULL_int col, const std::vector<double>& colNew);
    void set_column(ULL_int col, const std::vector<int>& colNew);
    void set_column(ULL_int col, double value);
    void set_column(ULL_int col);

    void insert_row(ULL_int row, const std::initializer_list<double>& rowNew);
    void insert_row(ULL_int row, const std::vector<double>& rowNew);
    void insert_row(ULL_int row, const std::vector<int>& rowNew);
    void insert_row(ULL_int row, double value);
    void insert_row(ULL_int row);

    void insert_column(ULL_int col, const std::initializer_list<double>& colNew);
    void insert_column(ULL_int col, const std::vector<double>& colNew);
    void insert_column(ULL_int col, const std::vector<int>& colNew);
    void insert_column(ULL_int col, double value);
    void insert_column(ULL_int col);

    void swap_row(ULL_int r1, ULL_int r2);
    void swap_column(ULL_int c1, ULL_int c2);
    void pop_back_row();
    void pop_back_column();
    void erase_row(ULL_int row);
    void erase_column(ULL_int col);
    void clear();

    void operator=(const Matrix& other);

    /* Binary math functions */

    Matrix operator+(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;

    Matrix operator*(double scale) const;
    friend Matrix operator*(double scale, const Matrix& rhs);
    Matrix operator*=(double scale);

    std::vector<double> operator*(const std::vector<double>& vector) const;
    std::vector<double> operator*(const std::vector<int>& vector) const;
    friend std::vector<double>
        operator*(const std::vector<double>& vector, const Matrix& rhs);
    friend std::vector<double>
        operator*(const std::vector<int>& vector, const Matrix& rhs);

    /* Uniary math functions */

    double determinant() const;
    Matrix transpose() const;
    Matrix rref() const;
    Matrix inverse() const;

    /* Output */

    friend std::ostream& operator<<(std::ostream &os, const Matrix& mat);
    void output_floatLen(unsigned int len); // broken

private:
    std::vector<std::vector<double>> _data;
    ULL_int _rows; // number of rows / size of columns
    ULL_int _columns; // number of columns / size of rows
    unsigned int _floatLen; // float length when printing matrix
    double _floatPrecis; // assumed float percision (based on _floatLen)

    bool _is_double_sub_zero(double value) const;
};

#endif