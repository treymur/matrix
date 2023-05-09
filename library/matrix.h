#pragma once
#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include <set>
// #include <initializer_list>  /* included in <vector> */

extern bool NICE_BRACKET;


class Matrix {
public:

    enum orientation {
        column,
        row
    };

    enum QR {
        Q,
        R
    };

    typedef std::pair<Matrix,Matrix> MatrixPair;

    /* Constructors */

    Matrix();
    Matrix(std::initializer_list<std::initializer_list<double>> in);
    Matrix(std::initializer_list<double> in, orientation type=column);
    template <typename T> 
    Matrix(const std::vector<std::vector<T>>& in);
    template <typename T> 
    Matrix(const std::vector<T>& in, orientation type=column);
    Matrix(const Matrix& other);
    Matrix(std::size_t rows, std::size_t columns, double value);
    Matrix(std::size_t rows, std::size_t columns);
    Matrix(std::size_t identitySize);
    
    /* Get functions */

    int num_rows() const;
    int num_columns() const;
    std::vector<double> get_row(std::size_t row) const;
    std::vector<double> get_column(std::size_t col) const;
    std::vector<double> to_vector() const;
    double& at(std::size_t row, std::size_t col);
    int size() const;
    bool empty() const;

    /* Edit functions */

    template <typename T> 
    void push_back_row(const std::vector<T>& row);
    void push_back_row(const std::initializer_list<double>& row);
    void push_back_row(double value);
    void push_back_row();

    template <typename T> 
    void push_back_column(const std::vector<T>& col);
    void push_back_column(const std::initializer_list<double>& col);
    void push_back_column(double value);
    void push_back_column();

    template <typename T>
    void set_row(std::size_t row, const std::vector<T>& rowNew);
    void set_row(std::size_t row, const std::initializer_list<double>& rowNew);
    void set_row(std::size_t row, double value);
    void set_row(std::size_t row);

    template <typename T>
    void set_column(std::size_t col, const std::vector<T>& colNew);
    void set_column(std::size_t col, const std::initializer_list<double>& colNew);
    void set_column(std::size_t col, double value);
    void set_column(std::size_t col);

    template <typename T>
    void insert_row(std::size_t row, const std::vector<T>& rowNew);
    void insert_row(std::size_t row, const std::initializer_list<double>& rowNew);
    void insert_row(std::size_t row, double value);
    void insert_row(std::size_t row);

    template <typename T>
    void insert_column(std::size_t col, const std::vector<T>& colNew);
    void insert_column(std::size_t col, const std::initializer_list<double>& colNew);
    void insert_column(std::size_t col, double value);
    void insert_column(std::size_t col);

    void swap_row(std::size_t r1, std::size_t r2);
    void swap_column(std::size_t c1, std::size_t c2);
    void pop_back_row();
    void pop_back_column();
    void erase_row(std::size_t row);
    void erase_column(std::size_t col);
    void clear();

    void augment(const Matrix& other, bool seperator=true);
    void operator=(const Matrix& other);

    /* Binary math functions */

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator+=(const Matrix& other);
    Matrix operator-=(const Matrix& other);

    Matrix operator*(const Matrix& other) const;

    Matrix operator*(double scale) const;
    friend Matrix operator*(double scale, const Matrix& rhs);
    Matrix operator*=(double scale);
    Matrix operator/(double scale) const;
    Matrix operator/=(double scale);

    template <typename T> 
    Matrix operator*(const std::vector<T>& vector) const;
    template <typename T> friend 
    Matrix operator*(const std::vector<T>& vector, const Matrix& rhs);
    
    double vec_dot() const;
    double vec_dot(const Matrix& other) const;

    /* Uniary math functions */

    double determinant() const;
    Matrix transpose() const;
    Matrix rref() const;
    Matrix inverse() const;
    MatrixPair qr() const;
    Matrix qr(QR output) const;
    std::vector<double> eigenvalues_approx(double percision=1e-12, 
                                           int max_iterations=100000) const;

    /* Output */

    friend std::ostream& operator<<(std::ostream &os, const Matrix& mat);
    void output_floatLen(unsigned int len); // broken

private:
    std::vector<std::vector<double>> _data;
    std::size_t _rows; // number of rows / size of columns
    std::size_t _columns; // number of columns / size of rows
    unsigned int _floatLen; // float length when printing matrix
    double _floatPrecis; // assumed float percision (based on _floatLen)
    std::set<std::size_t> _augment_lines; // location of any augment lines
    bool _niceBrackets = NICE_BRACKET; // weither to use upperscore in brackets
};

#endif