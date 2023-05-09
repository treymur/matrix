#include "matrix.h"
#include <stdexcept>
#include <iomanip>
#include <cmath> // sqrt()
#include <type_traits>

using namespace std;

#define DEF_FLOAT_LEN 4 // default float length
#define MAX_FLOAT_LEN 12 // max float length
#define MAX_MATRIX_SIZE 0x20000000 // 2^29

bool NICE_BRACKET = false;

#pragma region PRIVATE_FUNCTONS

/**
 * @brief checks if double is 0 (counting subnormal variables as 0)
 * 
 */
bool _is_double_sub_zero(double value) {
    return (fpclassify(value) == FP_SUBNORMAL) 
            || (fpclassify(value) == FP_ZERO);
}

#pragma endregion // PRIVATE_FUNCTONS
/******************************************************************************/
#pragma region CONSTRUCTORS

/**
 * @brief Construct an empty Matrix
 * 
 */
Matrix::Matrix() {
    _rows = 0;
    _columns = 0;
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}

/**
 * @brief Construct a Matrix with list of lists
 * 
 * @param in list of lists of doubles
 */
Matrix::Matrix(initializer_list<initializer_list<double>> in) {
    if(in.size()) { // if 'in' is not empty
        _columns = in.begin()->size(); // sets columns to size of first list
        _rows = 0;
        for(auto it = in.begin(); it != in.end(); ++it) {
            if(_columns != it->size()) { // check row length == columns
                throw invalid_argument("Rows must be same size");
            }
            vector<double> row;
            for(auto it2 = it->begin(); it2 != it->end(); ++it2) {
                row.push_back(*it2);
            }
            _data.push_back(row);
            ++_rows;
        }
    } else {
        _rows = 0;
        _columns = 0;
    }
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}
/**
 * @brief Construct a Matrix with vector of vectors
 * 
 * @param in vector<vector<T>>
 */
template <typename T>
Matrix::Matrix(const vector<vector<T>>& in) {
    static_assert(is_arithmetic<T>::value, "Vector must be arithmetic");
    if(in.size() > MAX_MATRIX_SIZE)
        throw out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(in.size()) { // if not empty
        if(in[0].size() > MAX_MATRIX_SIZE)
            throw out_of_range("size must be less than MAX_MATRIX_SIZE");
        _columns = in[0].size();
        for(vector<T> row : in)
            if(_columns != row.size())
                throw invalid_argument("Rows must be same size");
        _rows = in.size();
        for(vector<T> row : in) {
            _data.push_back(vector<double>(row.begin(), row.end()));
        }
    } else { // list empty
        _rows = 0;
        _columns = 0;
    }
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}

/**
 * @brief Construct a Matrix of one row/column with list
 * 
 * @param in list of numbers
 * @param type orientaion of vector: Matrix::column or Matrix::row
 */
Matrix::Matrix(initializer_list<double> in, orientation type) {
    if(in.size() > MAX_MATRIX_SIZE)
        throw out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(in.size()) { // if not empty
        if(type == column) {
            _columns = 1;
            _rows = in.size();
            for(auto it = in.begin(); it != in.end(); ++it) {
                _data.push_back(vector<double>({*it}));
            }
        } else if (type == row) {
            _rows = 1;
            _columns = in.size();
            _data.push_back(vector<double>(in.begin(), in.end()));
        } else
            throw invalid_argument("Invalid orientation parameter");

    } else { // list empty
        _rows = 0;
        _columns = 0;
    }
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}
/**
 * @brief Construct a Matrix of one row/column with list
 * 
 * @param in list of numbers
 * @param type orientaion of vector: Matrix::column or Matrix::row
 */
template <typename T> 
Matrix::Matrix(const vector<T>& in, orientation type) {
    if(in.size() > MAX_MATRIX_SIZE)
        throw out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(in.size()) { // if not empty
        if(type == column) {
            _columns = 1;
            _rows = in.size();
            for(auto it = in.begin(); it != in.end(); ++it) {
                _data.push_back(vector<double>({(double)*it}));
            }
        } else if (type == row) {
            _rows = 1;
            _columns = in.size();
            _data.push_back(vector<double>(in.begin(), in.end()));
        } else
            throw invalid_argument("Invalid orientation parameter");

    } else { // list empty
        _rows = 0;
        _columns = 0;
    }
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}

/**
 * @brief Construct a Matrix with other Matrix
 * 
 * @param other Matrix object
 */
Matrix::Matrix(const Matrix& other) {
    _columns = other._columns;
    _rows = other._rows;
    _data = other._data;
    _floatLen = other._floatLen;
    _floatPrecis = other._floatPrecis;
    _augment_lines = other._augment_lines;
}

/**
 * @brief Construct a Matrix with size and fill with value
 * 
 * @param rows number of rows in Matrix
 * @param columns number of columns in Matrix
 * @param value value to fill Matrix
 */
Matrix::Matrix(size_t rows, size_t columns, double value) {
    if(rows > MAX_MATRIX_SIZE || columns > MAX_MATRIX_SIZE)
        throw out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(columns == 0 || rows == 0) {
        _rows = 0;
        _columns = 0;
    } else {
        _rows = rows;
        _columns = columns;
        for(size_t i=0; i<rows; ++i) {
            _data.push_back(vector<double>(columns, value));
        }
    }
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}
/**
 * @brief Construct a Matrix with size and fill with 0's
 * 
 * @param rows number of rows in Matrix
 * @param columns number of columns in Matrix
 */
Matrix::Matrix(size_t rows, size_t columns) {
    if(rows > MAX_MATRIX_SIZE || columns > MAX_MATRIX_SIZE)
        throw out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(columns == 0 || rows == 0) {
        _rows = 0;
        _columns = 0;
    } else {
        _rows = rows;
        _columns = columns;
        for(size_t i=0; i<rows; ++i) {
            _data.push_back(vector<double>(columns));
        }
    }
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}

/**
 * @brief Construct a identity Matrix
 * 
 * @param identitySize size of rows/columns of identity matrix
 */
Matrix::Matrix(size_t identitySize) {
    if(identitySize > MAX_MATRIX_SIZE)
        throw out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(!identitySize) // if == 0
        throw invalid_argument("identity size must be greater than 0");
    _rows = identitySize;
    _columns = identitySize;
    for(size_t i=0; i<identitySize; ++i) {
        _data.push_back(vector<double>(identitySize));
        _data[i][i] = 1;
    }
    _floatLen = DEF_FLOAT_LEN;
    _floatPrecis = pow(10, -(DEF_FLOAT_LEN + 1));
}

#pragma endregion // CONTSRUCTORS
/******************************************************************************/
#pragma region GET_FUNCTIONS

/**
 * @return Number of rows in Matrix
 */
int Matrix::num_rows() const {
    return _rows;
}
/**
 * @return Number of columns in Matrix
 */
int Matrix::num_columns() const {
    return _columns;
}

/**
 * @param row row of Matrix to retrieve
 * @return vector<double> of row
 */
vector<double> Matrix::get_row(size_t row) const {
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    return _data[row];
}
/**
 * @param col column of Matrix to retrieve
 * @return vector<double> of column
 */
vector<double> Matrix::get_column(size_t col) const {
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    vector<double> column;
    for(vector<double> row : _data) {
        column.push_back(row[col]);
    }
    return column;
}

vector<double> Matrix::to_vector() const {
    if(empty())
        return vector<double>();
    if(_rows == 1) {
        return _data[0];
    } else if (_columns == 1) {
        vector<double> column;
        for(vector<double> row : _data) {
            column.push_back(row[0]);
        }
        return column;
    }
    throw invalid_argument("Must be only 1 row or column");
}

/**
 * @param row row of Matrix
 * @param col column of Matrix
 * @return value at given row and column
 */
double& Matrix::at(size_t row, size_t col) {
    if(row >= _rows || col >= _columns)
        throw out_of_range("Index does not exist");
    return _data[row][col];
}

/**
 * @return size of Matrix (rows * columns)
 */
int Matrix::size() const {
    return _rows * _columns;
}

/**
 * @return true if Matrix is empty
 */
bool Matrix::empty() const {
    return !(_rows || _columns);
}

#pragma endregion // GET_FUNCTIONS
/******************************************************************************/
#pragma region EDIT_FUNCTIONS

/**
 * @brief Adds row at bottom of Matrix
 * 
 * @param row list of doubles
 */
void Matrix::push_back_row(const initializer_list<double>& row) {    
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(row.begin() == row.end())
        throw invalid_argument("Row cannot be empty");
    if(empty())
        _columns = row.size();
    else if(_columns != row.size())
        throw invalid_argument("Row must be same size as Matrix rows");
    _data.push_back(row);
    ++_rows;
}
/**
 * @brief Adds row at bottom of Matrix
 * 
 * @param row vector
 */
template <typename T>
void Matrix::push_back_row(const vector<T>& row) {
    static_assert(is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(row.empty())
        throw invalid_argument("Row cannot be empty");
    if(empty())
        _columns = row.size();
    else if(_columns != row.size())
        throw invalid_argument("Row must be same size as Matrix rows");
    _data.push_back(vector<double>(row.begin(), row.end()));
    ++_rows;
}
/**
 * @brief Adds row at bottom of Matrix
 * 
 * @param value containts of new row
 */
void Matrix::push_back_row(double value) {
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(empty())
        throw domain_error("Must have data to add row without size");
    vector<double> row(_columns, value);
    _data.push_back(row);
    ++_rows;
}
/**
 * @brief Adds blank row at bottom of Matrix
 * 
 */
void Matrix::push_back_row() {
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(empty())
        throw domain_error("Must have data to add row without size");
    vector<double> row(_columns);
    _data.push_back(row);
    ++_rows;
}

/**
 * @brief Adds column at right edge of Matrix
 * 
 * @param col list of doubles
 */
void Matrix::push_back_column(const initializer_list<double>& col) {
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(col.begin() == col.end())
        throw invalid_argument("Column cannot be empty");
    if(empty()) {
        _rows = col.size();
        for(size_t i=0; i<_rows; ++i)
            _data.push_back(vector<double>());
    } else if(_rows != col.size())
        throw 
            invalid_argument("Column must be same size as Matrix columns");
    size_t index = 0;
    for(auto iter = col.begin(); iter != col.end(); ++iter) {
        _data[index++].push_back(*iter);
    }
    ++_columns;
}
/**
 * @brief Adds column at right edge of Matrix
 * 
 * @param col vector of ints
 */
template <typename T>
void Matrix::push_back_column(const vector<T>& col) {
    static_assert(is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(col.empty())
        throw invalid_argument("Column cannot be empty");
    if(empty()) {
        _rows = col.size();
        for(size_t i=0; i<_rows; ++i)
            _data.push_back(vector<double>());
    } else if(_rows != col.size())
        throw 
            invalid_argument("Column must be same size as Matrix columns");
    vector<double> colNew;
    size_t index = 0;
    for(auto iter = col.begin(); iter != col.end(); ++iter) {
        _data[index++].push_back(*iter);
    }
    ++_columns;
}
/**
 * @brief Adds column at right edge of Matrix
 * 
 * @param value containts of new row
 */
void Matrix::push_back_column(double value) {
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(empty())
        throw domain_error("Must have data to add column without size");
    for(auto iter = _data.begin(); iter != _data.end(); ++iter) {
        iter->push_back(value);
    }
    ++_columns;
}
/**
 * @brief Adds blank column at right edge of Matrix
 * 
 */
void Matrix::push_back_column() {
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(empty())
        throw domain_error("Must have data to add column without size");
    for(auto iter = _data.begin(); iter != _data.end(); ++iter) {
        iter->push_back(0);
    }
    ++_columns;
}

/**
 * @brief Replace row of Matrix at given index
 * 
 * @param row row index of Matrix
 * @param rowNew new row (list)
 */
void Matrix::set_row(size_t row, const initializer_list<double>& rowNew) {
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw invalid_argument("Row must be same size as Matrix rows");
    _data[row] = rowNew;
}
/**
 * @brief Replace row of Matrix at given index
 * 
 * @param row row index of Matrix
 * @param rowNew new row (vector<int>)
 */
template <typename T>
void Matrix::set_row(size_t row, const vector<T>& rowNew) {
    static_assert(is_arithmetic<T>::value, "Vector must be arithmetic");
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw invalid_argument("Row must be same size as Matrix rows");
    _data[row] = vector<double>(rowNew.begin(), rowNew.end());
}
/**
 * @brief Replace row of Matrix at given index
 * 
 * @param row row index of Matrix
 * @param value values in new row
 */
void Matrix::set_row(size_t row, double value) {
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    for(size_t i=0; i<_columns; ++i) {
        _data[row][i] = value;
    }
}
/**
 * @brief Replace row of Matrix with 0's at given index
 * 
 * @param row row index of Matrix
 */
void Matrix::set_row(size_t row) {
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    for(size_t i=0; i<_columns; ++i) {
        _data[row][i] = 0;
    }
}

/**
 * @brief Replace column of Matrix at given index
 * 
 * @param col column index of Matrix
 * @param colNew new column (list)
 */
void Matrix::set_column(size_t col, const initializer_list<double>& colNew){
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            invalid_argument("Column must be same size as Matrix columns");
    size_t index = 0;
    for(auto iter = colNew.begin(); iter != colNew.end(); ++iter) {
        _data[index++][col] = *iter;
    }
}
/**
 * @brief Replace column of Matrix at given index
 * 
 * @param col column index of Matrix
 * @param colNew new column (vector<int>)
 */
template <typename T>
void Matrix::set_column(size_t col, const vector<T>& colNew){
    static_assert(is_arithmetic<T>::value, "Vector must be arithmetic");
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            invalid_argument("Column must be same size as Matrix columns");
    size_t index = 0;
    for(auto iter = colNew.begin(); iter != colNew.end(); ++iter) {
        _data[index++][col] = *iter;
    }
}
/**
 * @brief Replace column of Matrix at given index
 * 
 * @param col column index of Matrix
 * @param value values in new column
 */
void Matrix::set_column(size_t col, double value) {
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    for(size_t i=0; i<_rows; ++i) {
        _data[i][col] = value;
    }
}
/**
 * @brief Replace column of Matrix with 0's at given index
 * 
 * @param col column index of Matrix
 */
void Matrix::set_column(size_t col) {
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    for(size_t i=0; i<_rows; ++i) {
        _data[i][col] = 0;
    }
}

/**
 * @brief Insert row at index and shift rows below
 * 
 * @param row index of Matrix to insert row
 * @param rowNew new row (list)
 */
void Matrix::insert_row(size_t row, const initializer_list<double>& rowNew) {
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw invalid_argument("Row must be same size as Matrix rows");
    _data.insert(_data.begin() + row, rowNew);
    ++_rows;
}
/**
 * @brief Insert row at index and shift rows below
 * 
 * @param row index of Matrix to insert row
 * @param rowNew new row (vector<int>)
 */
template <typename T>
void Matrix::insert_row(size_t row, const vector<T>& rowNew) {
    static_assert(is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw invalid_argument("Row must be same size as Matrix rows");
    _data.insert(_data.begin() + row, 
                 vector<double>(rowNew.begin(), rowNew.end()));
    ++_rows;
}
/**
 * @brief Insert row of values at index and shift rows below
 * 
 * @param row index of Matrix to insert row
 * @param value value to fill row
 */
void Matrix::insert_row(size_t row, double value) {
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    _data.insert(_data.begin() + row, vector<double>(_columns, value));
    ++_rows;
}
/**
 * @brief Insert row of 0s at index and shift rows below
 * 
 * @param row index of Matrix to insert row
 */
void Matrix::insert_row(size_t row) {
    if(_rows == MAX_MATRIX_SIZE)
        throw out_of_range("rows at max size");
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    _data.insert(_data.begin() + row, vector<double>(_columns));
    ++_rows;
}

/**
 * @brief Insert column at index and shift columns on the right
 * 
 * @param col index of Matrix to insert column
 * @param colNew new column (list)
 */
void Matrix::insert_column(size_t col, const initializer_list<double>& colNew) {
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            invalid_argument("Column must be same size as Matrix columns");
    size_t index = 0;
    for(auto iter = colNew.begin(); iter != colNew.end(); ++iter, ++index) {
        _data[index].insert(_data[index].begin() + col, *iter);
    }
    ++_columns;
}
/**
 * @brief Insert column at index and shift columns on the right
 * 
 * @param col index of Matrix to insert column
 * @param colNew new column (vector<int>)
 */
template <typename T>
void Matrix::insert_column(size_t col, const vector<T>& colNew) {
    static_assert(is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            invalid_argument("Column must be same size as Matrix columns");
    size_t index = 0;
    for(auto iter = colNew.begin(); iter != colNew.end(); ++iter, ++index) {
        _data[index].insert(_data[index].begin() + col, *iter);
    }
    ++_columns;
}
/**
 * @brief Insert column of values at index and shift columns on the right
 * 
 * @param col index of Matrix to insert column
 * @param value value to fill column
 */
void Matrix::insert_column(size_t col, double value) {
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    size_t index = 0;
    for(auto iter = _data.begin(); iter != _data.end(); ++iter, ++index) {
        iter->insert(iter->begin() + col, value);
    }
    ++_columns;
}
/**
 * @brief Insert column of 0s at index and shift columns on the right
 * 
 * @param col index of Matrix to insert column
 */
void Matrix::insert_column(size_t col) {
    if(_columns == MAX_MATRIX_SIZE)
        throw out_of_range("columns at max size");
    if(col >= _columns)
        throw out_of_range("Column does not exist");
    size_t index = 0;
    for(auto iter = _data.begin(); iter != _data.end(); ++iter, ++index) {
        iter->insert(iter->begin() + col, 0);
    }
    ++_columns;
}



/**
 * @brief Swaps 2 rows of given indexes of Matrix
 * 
 * 
 */
void Matrix::swap_row(size_t r1, size_t r2) {
    if(r1 >= _rows || r2 >= _rows)
        throw out_of_range("Row does not exist");
    vector<double> tmp = _data[r1];
    _data[r1] = _data[r2];
    _data[r2] = tmp;
}
/**
 * @brief Swaps 2 columns of given indexes of Matrix
 * 
 * 
 */
void Matrix::swap_column(size_t c1, size_t c2) {
    if(c1 >= _columns || c2 >= _columns)
        throw out_of_range("Column does not exist");
    vector<double> tmp;
    for(size_t i=0; i<_rows; ++i) {
        tmp.push_back(_data[i][c1]);
        _data[i][c1] = _data[i][c2];
        _data[i][c2] = tmp[i];
    }
}

/**
 * @brief Deletes last row
 * 
 */
void Matrix::pop_back_row() {
    if(empty())
        throw domain_error("No values to pop");
    if(_rows == 1) {
        clear();
    } else {
        _data.pop_back();
        --_rows;
    }
}
/**
 * @brief Deletes last column
 * 
 */
void Matrix::pop_back_column() {
    if(empty())
        throw domain_error("No values to pop");
    if(_columns == 1) {
        clear();
    } else {
        for(auto iter = _data.begin(); iter != _data.end(); ++iter) {
            iter->pop_back();
        }
        --_columns;
    }
}

/**
 * @brief Erases row
 * 
 * @param row index of row in Matrix
 */
void Matrix::erase_row(size_t row) {
    if(empty())
        throw domain_error("No values to erase");
    if(row >= _rows)
        throw out_of_range("Row does not exist");
    if(_rows == 1) {
        clear();
    } else {
        _data.erase(_data.begin() + row);
        --_rows;
    }
}
/**
 * @brief Erases column
 * 
 * @param col index of column in Matrix
 */
void Matrix::erase_column(size_t col) {
    if(empty())
        throw domain_error("No values to erase");
    if(col >= _columns)
        throw out_of_range("Row does not exist");
    if(_columns == 1) {
        clear();
    } else {
        for(auto iter = _data.begin(); iter != _data.end(); ++iter) {
            iter->erase(iter->begin() + col);
        }
        --_columns;
    }
}

/**
 * @brief clear Matrix
 * 
 */
void Matrix::clear() {
    _columns = 0;
    _rows = 0;
    _data.clear();
}

/**
 * @brief augments matrix to the right side of current matrix
 * 
 * @param other matrix to add
 */
void Matrix::augment(const Matrix& other, bool seperator) {
    if(empty()) {
        *this = other;
        return;
    }
    if(_rows != other._rows)
        throw invalid_argument("Matricies must have same column length");
    if(seperator)
        _augment_lines.insert(_columns);
    _columns += other._columns;
    for(size_t i=0; i<_rows; ++i) {
        for(double x : other._data[i]) {
            _data[i].push_back(x);
        }
    }
}

/**
 * @brief set Matrix to be the same as other (does not change float lenght)
 * 
 */
void Matrix::operator=(const Matrix& other) {
    _columns = other._columns;
    _rows = other._rows;
    _data = other._data;
    _augment_lines = other._augment_lines;
}


#pragma endregion // EDIT_FUNCTIONS
/******************************************************************************/
#pragma region BINARY_MATH_FUNCTIONS

/**
 * @brief Add Matricies with equal dimentions
 * 
 */
Matrix Matrix::operator+(const Matrix& other) const {
    if(empty() || other.empty())
        throw domain_error("Matricies must have data");
    if(other._columns != _columns || other._rows != _rows)
        throw invalid_argument
            ("Matricies must have same dimentions for addition");
    Matrix sum;
    for(size_t i=0; i<_rows; ++i) {
        vector<double> rowNew;
        for(size_t j=0; j<_columns; ++j) {
            rowNew.push_back(_data[i][j] + other._data[i][j]);
        }
        sum.push_back_row(rowNew);
    }
    return sum;
}
/**
 * @brief Subtract Matricies with equal dimentions
 * 
 */
Matrix Matrix::operator-(const Matrix& other) const {
    if(empty() || other.empty())
        throw domain_error("Matricies must have data");
    if(other._columns != _columns || other._rows != _rows)
        throw invalid_argument
            ("Matricies must have same dimentions for subtraction");
    Matrix sum;
    for(size_t i=0; i<_rows; ++i) {
        vector<double> rowNew;
        for(size_t j=0; j<_columns; ++j) {
            rowNew.push_back(_data[i][j] - other._data[i][j]);
        }
        sum.push_back_row(rowNew);
    }
    return sum;
}
/**
 * @brief Incriment Matrix by another Matrix with equal dimentions
 * 
 */
Matrix Matrix::operator+=(const Matrix& other) {
    *this = *this + other;
    return *this;
}
/**
 * @brief Deincriment Matrix by another Matrix with equal dimentions
 * 
 */
Matrix Matrix::operator-=(const Matrix& other) {
    *this = *this - other;
    return *this;
}

/**
 * @brief Multiply Matricices
 * 
 */
Matrix Matrix::operator*(const Matrix& other) const {
    if(empty() || other.empty())
        throw domain_error("Matricies must have data");
    if(_columns != other._rows)
        throw invalid_argument
            ("Invalid Matrix dimentions for multiplication");
    Matrix product;
    for(size_t i=0; i<_rows; ++i) {
        vector<double> rowNew;
        for(size_t j=0; j<other._columns; ++j) {
            double sum = 0;
            for(size_t k=0; k<_columns; ++k) {
                sum += _data[i][k] * other._data[k][j];
            }
            rowNew.push_back(sum);
        }
        product.push_back_row(rowNew);
    }
    return product;
}

/**
 * @brief multiply Matrix by scale
 * 
 */
Matrix Matrix::operator*(double scale) const {
    if(empty())
        throw domain_error("Matrix must have data");
    Matrix product;
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        vector<double> rowNew;
        for(auto iter = rowIter->begin(); iter != rowIter->end(); ++iter) {
            rowNew.push_back(*iter * scale);
        }
        product.push_back_row(rowNew);
    }
    return product;
}
/**
 * @brief multiply Matrix by scale
 * 
 */
Matrix operator*(double scale, const Matrix& rhs) {
    return rhs * scale;
}
/**
 * @brief multiply Matrix by scale
 * 
 */
Matrix Matrix::operator*=(double scale) {
    if(empty())
        throw domain_error("Matrix must have data");
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        for(auto iter = rowIter->begin(); iter != rowIter->end(); ++iter) {
            *iter *= scale;
        }
    }
    return *this;
}

/**
 * @brief divide Matrix by scale
 * 
 */
Matrix Matrix::operator/(double scale) const {
    if(scale == 0)
        throw invalid_argument("scale cannot be zero");
    if(empty())
        throw domain_error("Matrix must have data");
    Matrix quotient;
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        vector<double> rowNew;
        for(auto iter = rowIter->begin(); iter != rowIter->end(); ++iter) {
            rowNew.push_back(*iter / scale);
        }
        quotient.push_back_row(rowNew);
    }
    return quotient;
}
/**
 * @brief divide Matrix by scale
 * 
 */
Matrix Matrix::operator/=(double scale) {
    if(scale == 0)
        throw invalid_argument("scale cannot be zero");
    if(empty())
        throw domain_error("Matrix must have data");
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        for(auto iter = rowIter->begin(); iter != rowIter->end(); ++iter) {
            *iter /= scale;
        }
    }
    return *this;
}

/**
 * @brief multiply Matrix with vector
 * 
 */
template <typename T>
Matrix Matrix::operator*(const vector<T>& vec) const {
    if(empty())
        throw domain_error("Matrix must have data");
    if(_columns != vec.size())
        throw invalid_argument("Vector must be same size as number of columns");
    vector<double> product;
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        double sum = 0;
        size_t index = 0;
        for(auto iter = vec.begin(); iter != vec.end(); ++iter) {
            sum += *iter * rowIter->at(index++);
        }
        product.push_back(sum);
    }
    return Matrix(product);
}
/**
 * @brief multiply Matrix with vector
 * 
 */
template <typename T>
Matrix operator*(const vector<T>& vector, const Matrix& rhs) {
    return (rhs.transpose() * vector).transpose();
}

/**
 * @brief Takes dot product of vector with itself
 * 
 */
double Matrix::vec_dot() const {
    double sum = 0;
    if(_columns == 1) {
        for(size_t i=0; i<_rows; ++i) {
            sum += _data[i][0] * _data[i][0];
        }
        return sum;
    } else if(_rows == 1) {
        for(size_t i=0; i<_columns; ++i) {
            sum += _data[0][i] * _data[0][i];
        }
        return sum;
    }
    throw invalid_argument("Must use vector");
}
/**
 * @brief Takes dot product of two vector matricies
 * 
 */
double Matrix::vec_dot(const Matrix& other) const {
    vector<double> leftVec, rightVec;
    if(_columns == 1) {
        leftVec = get_column(0);
    } else if(_rows == 1) {
        leftVec = get_row(0);
    } else {
        throw invalid_argument("Must use vectors");
    }
    if(other._columns == 1) {
        rightVec = other.get_column(0);
    } else if(other._rows == 1) {
        rightVec = other.get_row(0);
    } else {
        throw invalid_argument("Must use vectors");
    }
    if(leftVec.size() != rightVec.size())
        throw invalid_argument("Vectors must be same size");
    double sum = 0;
    for(size_t i=0; i<leftVec.size(); ++i) {
        sum += leftVec[i] * rightVec[i];
    }
    return sum;
}

#pragma endregion // BINARY_MATH_FUNCTIONS
/******************************************************************************/
#pragma region UNIARY_MATH_FUNCTIONS

/**
 * @brief returns determinate of Matrix
 * 
 */
double Matrix::determinant() const{
    if(empty())
        throw invalid_argument("Matrix must have data");
    if(_columns != _rows)
        throw invalid_argument("Matrix must be square");
    Matrix M(_data);
    double scale = 1;
    for(size_t i=0; i<_columns; ++i) {
        size_t row = i; // starting row == number of past loops
        while(_is_double_sub_zero(M._data[row][i])) {
            if(++row == _rows) { // if at end, means at least one 0 on diagonal
                return 0;
            }
        }
        if(row != i) {
            M.swap_row(row, i);
            scale *= -1;
        }
        for(row=i+1; row<_rows; ++row) { // sweep rows below
            if(!_is_double_sub_zero(M._data[row][i])) {
                double coeff = M._data[row][i] / M._data[i][i];
                for(size_t j=i; j<_columns; ++j) {
                    M._data[row][j] -= coeff * M._data[i][j];
                }
            }
        }
    }
    for(size_t i=0; i<_rows; ++i) { // multiply diagonal
        scale *= M._data[i][i];
    }
    return scale;
}


/**
 * @brief returns transpose of Matrix
 * 
 */
Matrix Matrix::transpose() const {
    if(empty())
        throw invalid_argument("Matrix cannot be empty");
    Matrix M;
    for(size_t i = 0; i<_columns; ++i) {
        M.push_back_row(get_column(i));
    }
    return M;
}

/**
 * @brief Computes reduced row echelon form of Matrix with Gaussian elimination
 *  
 */
Matrix Matrix::rref() const {
    if(empty())
        throw invalid_argument("Matrix cannot be empty");
    Matrix M = *this;
    size_t lead = 0; // column of current leading value
    for(size_t i=0; i<_rows && lead<_columns; ++i) {
        size_t row = i; // current row (start at top of unchanged lead values)
        while(_is_double_sub_zero(M._data[row][lead])) { // while zero
            ++row;
            if(row == _rows) {
                row = i;
                ++lead;
                if(lead == _columns) {
                    return M;
                }
            }
        }
        swap(M._data[row], M._data[i]);
        double *leadingVals = new double[_rows]; // leading values at col: lead
        for(size_t k=0; k<_rows; ++k) {
            leadingVals[k] = M._data[k][lead];
        }
        for(size_t j=lead; j<_columns; ++j) {
            M._data[i][j] /= leadingVals[i];
            double tmp = M._data[i][j];
            for(size_t k=0; k<_rows; ++k) {
                if(k != i && !_is_double_sub_zero(leadingVals[k])) {
                    M._data[k][j] -= leadingVals[k] * tmp;
                }
            }
        }
        delete[] leadingVals;
        ++lead;
    }
    return M;
}

/**
 * @brief returns inverse of Matrix
 * 
 */
Matrix Matrix::inverse() const {
    if(empty())
        throw invalid_argument("Matrix must have data");
    if(_columns != _rows)
        throw invalid_argument("Matrix must be square");
    if(_is_double_sub_zero(determinant())) {
        cerr << "Matrix not invertable";
        return Matrix();
    }
    Matrix M(_data);
    size_t len = _rows;
    for(size_t i=0; i<len; ++i) {
        vector<double> idCol(len);
        idCol[i] = 1;
        M.push_back_column(idCol);
    }
    Matrix I = M.rref();
    for(size_t i=0; i<len; ++i) {
        I.erase_column(0);
    }
    return I;
}

/**
 * @brief Finds QR decompisition of Matrix
 * 
 * @return pair<Matrix,Matrix>; first = Q, second = R 
 */
Matrix::MatrixPair Matrix::qr() const {
    if(empty())
        throw invalid_argument("Matrix must have data");
    Matrix Q_matrix, R_matrix(_columns, _columns);
    for(size_t i=0; i<_columns; ++i) {
        Matrix col = get_column(i);
        Matrix colPerp = col;
        for(size_t j=0; j<i; ++j) {
            Matrix qCol = Q_matrix.get_column(j);
            float dot = col.vec_dot(qCol);
            R_matrix.at(j, i) = dot;
            colPerp -= qCol * dot;
        }
        float colPerpLen = sqrt(colPerp.vec_dot());
        if(colPerpLen == 0) {
            throw invalid_argument("Columns must be linearly independant");
        }
        R_matrix.at(i, i) = colPerpLen;
        Q_matrix.augment(colPerp/colPerpLen, false);
    }
    return MatrixPair(Q_matrix,R_matrix);
}
/**
 * @brief Returns Q or R from QR decompisition
 * 
 * @param output Which matrix to output (Matrix::Q or Matrix::R)
 */
Matrix Matrix::qr(QR output) const {
    if(empty())
        throw invalid_argument("Matrix must have data");
    if(output == Q) {
        Matrix Q_matrix;
        for(size_t i=0; i<_columns; ++i) {
            Matrix col = get_column(i);
            Matrix colPerp = col;
            for(size_t j=0; j<i; ++j) {
                Matrix qCol = Q_matrix.get_column(j);
                colPerp -= qCol * col.vec_dot(qCol);
            }
            float colPerpLen = sqrt(colPerp.vec_dot());
            if(colPerpLen == 0) {
                throw invalid_argument("Columns must be linearly independant");
            }
            Q_matrix.augment(colPerp/colPerpLen, false);
        }
        return Q_matrix;
    }
    if(output == R) {
        return qr().second;
    }
    throw invalid_argument("Invalid param must be Matrix::Q or Matrix::R");
}

/**
 * @brief Finds eigenvalues of Matrix if all real as vector<double>
 * 
 * @param percision defines upper matrix with values below diagnol < percision
 *                  (defaults to 10^-12)
 * @param max_iterations max number of iterations of QR algorithm
 */
vector<double> 
Matrix::eigenvalues_approx(double percision, int max_iterations) const {
    if(empty())
        throw invalid_argument("Matrix must have data");
    if(_columns != _rows)
        throw invalid_argument("Matrix must be square");
    bool is_upper = false;
    Matrix tmp = _data;
    int count;
    for(count=0; !is_upper && count<max_iterations; ++count) {
        MatrixPair QR = tmp.qr();
        tmp = QR.second * QR.first;
        is_upper = true;
        for(size_t row=0; row<_rows && is_upper; row++) {
            for(size_t col=0; col<row && is_upper; col++) {
                if(abs(tmp._data[row][col]) > percision) {
                    is_upper = false;
                }
            }
        }
    }
    if(!is_upper)
        throw runtime_error("Could not find values, could be imaginary");
    vector<double> output;
    for(size_t i=0; i<_rows; ++i) {
        output.push_back(tmp._data[i][i]);
    }
    return output;
}

#pragma endregion // UNIARY_MATH_FUNCTIONS
/******************************************************************************/
#pragma region OUTPUT

/**
 * @brief beutify Matrix to print
 * 
 * @param os output stream
 * @param mat Matrix object 
 */
ostream& operator<<(ostream &os, const Matrix& mat) {
    /* Find max integer lenghts of each column */
    int *intMaxLen = new int[mat._columns]; // set to 1s
    for(size_t i=0; i<mat._columns; ++i) {
        intMaxLen[i] = 0;
    }
    bool allInt = true;
    for(vector<double> row : mat._data) {
        for(size_t i=0; i<mat._columns; ++i) {
            double intPart, floatPart = modf(row[i], &intPart);
            int intLen = to_string((int)(intPart+0.5-(row[i]<0))).length();
            if((int)(intPart+0.5-(row[i]<0)) == 0 && row[i] < 0) {
                ++intLen;
            }
            if(intLen > intMaxLen[i]) {
                intMaxLen[i] = intLen;
            }
            if(abs(floatPart + mat._floatPrecis) > 2 * mat._floatPrecis) {
                allInt = false;
            }
        }
    }
    int float_length = 0;
    if(!allInt) {
        float_length = mat._floatLen;
    }
    os << fixed << setprecision(float_length);
    for(size_t row=0; row<mat._rows; ++row) {
        os << "|";
        if(mat._niceBrackets && row == 0)
            os << "‾";
        else if(row == mat._rows-1)
            os << "_"; 
        else
            os << " ";
        for(size_t col=0; col<mat._columns; ++col) {
            if(mat._augment_lines.find(col) != mat._augment_lines.end())
                os << "|";
            os << " " 
               << setw(intMaxLen[col] + float_length + (float_length != 0)); 
            if(_is_double_sub_zero(mat._data[row][col]))
                os << setprecision(0) << 0 
                   << setprecision(float_length) << " ";
            else
                os << mat._data[row][col] << " ";
            // if(col != mat._columns-1)
            //     os << "  ";
        }
        if(mat._niceBrackets && row == 0)
            os << "‾|" << endl;
        else if(row == mat._rows-1)
            os << "_|" << endl;
        else
            os << " |" << endl;
    }
    delete[] intMaxLen;
    return os;
}

/**
 * @brief (BROKEN) set float lenth of values when printing
 * 
 */
void Matrix::output_floatLen(unsigned int len) {
    if(len > MAX_FLOAT_LEN)
        throw invalid_argument
            ("float length must be less than MAX_FLOAT_LEN");
    _floatLen = len;
    _floatPrecis = pow(10, -(_floatLen + 1));
        
}

#pragma endregion // OUTPUT
/******************************************************************************/
#pragma region EXPLICIT_INSTANTIATIONS

/* int */
template Matrix::Matrix(const vector<vector<int>>& in);
template Matrix::Matrix(const vector<int>& in, orientation type);
template void Matrix::push_back_row(const vector<int>& row);
template void Matrix::push_back_column(const vector<int>& col);
template void Matrix::set_row(size_t row, const vector<int>& rowNew);
template void Matrix::set_column(size_t col, const vector<int>& colNew);
template void Matrix::insert_row(size_t row, const vector<int>& rowNew);
template void Matrix::insert_column(size_t col, const vector<int>& colNew);
template Matrix Matrix::operator*(const vector<int>& vector) const;
template Matrix operator*(const vector<int>& vector, const Matrix& rhs);


/* double */
template Matrix::Matrix(const vector<vector<double>>& in);
template Matrix::Matrix(const vector<double>& in, orientation type);
template void Matrix::push_back_row(const vector<double>& row);
template void Matrix::push_back_column(const vector<double>& col);
template void Matrix::set_row(size_t row, const vector<double>& rowNew);
template void Matrix::set_column(size_t col, const vector<double>& colNew);
template void Matrix::insert_row(size_t row, const vector<double>& rowNew);
template void Matrix::insert_column(size_t col, const vector<double>& colNew);
template Matrix Matrix::operator*(const vector<double>& vector) const;
template Matrix operator*(const vector<double>& vector, const Matrix& rhs);


/* float */
template Matrix::Matrix(const vector<vector<float>>& in);
template Matrix::Matrix(const vector<float>& in, orientation type);
template void Matrix::push_back_row(const vector<float>& row);
template void Matrix::push_back_column(const vector<float>& col);
template void Matrix::set_row(size_t row, const vector<float>& rowNew);
template void Matrix::set_column(size_t col, const vector<float>& colNew);
template void Matrix::insert_row(size_t row, const vector<float>& rowNew);
template void Matrix::insert_column(size_t col, const vector<float>& colNew);
template Matrix Matrix::operator*(const vector<float>& vector) const;
template Matrix operator*(const vector<float>& vector, const Matrix& rhs);

#pragma endregion // EXPLICIT_INSTANTIATIONS
