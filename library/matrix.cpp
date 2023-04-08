#include "matrix.h"
#include <stdexcept>
#include <iomanip>
#include <cmath>
#include <type_traits>


#define DEF_FLOAT_LEN 4 // default float length
#define MAX_FLOAT_LEN 12 // max float length
#define MAX_MATRIX_SIZE 0x20000000 // 2^29

#pragma region PRIVATE_FUNCTONS

/**
 * @brief checks if double is 0 (counting subnormal variables as 0)
 * 
 */
bool _is_double_sub_zero(double value) {
    return (std::fpclassify(value) == FP_SUBNORMAL) 
            || (std::fpclassify(value) == FP_ZERO);
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
Matrix::Matrix(std::initializer_list<std::initializer_list<double>> in) {
    if(in.size()) { // if 'in' is not empty
        _columns = in.begin()->size(); // sets columns to size of first list
        _rows = 0;
        for(auto it = in.begin(); it != in.end(); ++it) {
            if(_columns != it->size()) { // check row length == columns
                throw std::invalid_argument("Rows must be same size");
            }
            std::vector<double> row;
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
Matrix::Matrix(const std::vector<std::vector<T>>& in) {
    static_assert(std::is_arithmetic<T>::value, "Vector must be arithmetic");
    if(in.size() > MAX_MATRIX_SIZE)
        throw std::out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(in.size()) { // if not empty
        if(in[0].size() > MAX_MATRIX_SIZE)
            throw std::out_of_range("size must be less than MAX_MATRIX_SIZE");
        _columns = in[0].size();
        for(std::vector<T> row : in)
            if(_columns != row.size())
                throw std::invalid_argument("Rows must be same size");
        _rows = in.size();
        for(std::vector<T> row : in) {
            _data.push_back(std::vector<double>(row.begin(), row.end()));
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
 * @param type orientaion of vector: (c)olumn/(v)ertical or (r)ow/(h)orizontal
 */
Matrix::Matrix(std::initializer_list<double> in, orientation type) {
    if(in.size() > MAX_MATRIX_SIZE)
        throw std::out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(in.size()) { // if not empty
        if(type == column) {
            _columns = 1;
            _rows = in.size();
            for(auto it = in.begin(); it != in.end(); ++it) {
                _data.push_back(std::vector<double>({*it}));
            }
        } else if (type == row) {
            _rows = 1;
            _columns = in.size();
            _data.push_back(std::vector<double>(in.begin(), in.end()));
        } else
            throw std::invalid_argument("Invalid orientation parameter");

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
 * @param type orientaion of vector: (c)olumn/(v)ertical or (r)ow/(h)orizontal
 */
template <typename T> 
Matrix::Matrix(const std::vector<T>& in, orientation type) {
    if(in.size() > MAX_MATRIX_SIZE)
        throw std::out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(in.size()) { // if not empty
        if(type == column) {
            _columns = 1;
            _rows = in.size();
            for(auto it = in.begin(); it != in.end(); ++it) {
                _data.push_back(std::vector<double>({(double)*it}));
            }
        } else if (type == row) {
            _rows = 1;
            _columns = in.size();
            _data.push_back(std::vector<double>(in.begin(), in.end()));
        } else
            throw std::invalid_argument("Invalid orientation parameter");

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
}

/**
 * @brief Construct a Matrix with size and fill with value
 * 
 * @param rows number of rows in Matrix
 * @param columns number of columns in Matrix
 * @param value value to fill Matrix
 */
Matrix::Matrix(ULL_int rows, ULL_int columns, double value) {
    if(rows > MAX_MATRIX_SIZE || columns > MAX_MATRIX_SIZE)
        throw std::out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(columns == 0 || rows == 0) {
        _rows = 0;
        _columns = 0;
    } else {
        _rows = rows;
        _columns = columns;
        for(ULL_int i=0; i<rows; ++i) {
            _data.push_back(std::vector<double>(columns, value));
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
Matrix::Matrix(ULL_int rows, ULL_int columns) {
    if(rows > MAX_MATRIX_SIZE || columns > MAX_MATRIX_SIZE)
        throw std::out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(columns == 0 || rows == 0) {
        _rows = 0;
        _columns = 0;
    } else {
        _rows = rows;
        _columns = columns;
        for(ULL_int i=0; i<rows; ++i) {
            _data.push_back(std::vector<double>(columns));
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
Matrix::Matrix(ULL_int identitySize) {
    if(identitySize > MAX_MATRIX_SIZE)
        throw std::out_of_range("size must be less than MAX_MATRIX_SIZE");
    if(!identitySize) // if == 0
        throw std::invalid_argument("identity size must be greater than 0");
    _rows = identitySize;
    _columns = identitySize;
    for(ULL_int i=0; i<identitySize; ++i) {
        _data.push_back(std::vector<double>(identitySize));
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
std::vector<double> Matrix::get_row(ULL_int row) const {
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    return _data[row];
}
/**
 * @param col column of Matrix to retrieve
 * @return vector<double> of column
 */
std::vector<double> Matrix::get_column(ULL_int col) const {
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    std::vector<double> column;
    for(std::vector<double> row : _data) {
        column.push_back(row[col]);
    }
    return column;
}

std::vector<double> Matrix::to_vector() const {
    if(empty())
        return std::vector<double>();
    if(_rows == 1) {
        return _data[0];
    } else if (_columns == 1) {
        std::vector<double> column;
        for(std::vector<double> row : _data) {
            column.push_back(row[0]);
        }
        return column;
    }
    throw std::invalid_argument("Must be only 1 row or column");
}

/**
 * @param row row of Matrix
 * @param col column of Matrix
 * @return value at given row and column
 */
double& Matrix::at(ULL_int row, ULL_int col) {
    if(row >= _rows || col >= _columns)
        throw std::out_of_range("Index does not exist");
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
void Matrix::push_back_row(const std::initializer_list<double>& row) {    
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(row.begin() == row.end())
        throw std::invalid_argument("Row cannot be empty");
    if(empty())
        _columns = row.size();
    else if(_columns != row.size())
        throw std::invalid_argument("Row must be same size as Matrix rows");
    _data.push_back(row);
    ++_rows;
}
/**
 * @brief Adds row at bottom of Matrix
 * 
 * @param row vector
 */
template <typename T>
void Matrix::push_back_row(const std::vector<T>& row) {
    static_assert(std::is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(row.empty())
        throw std::invalid_argument("Row cannot be empty");
    if(empty())
        _columns = row.size();
    else if(_columns != row.size())
        throw std::invalid_argument("Row must be same size as Matrix rows");
    _data.push_back(std::vector<double>(row.begin(), row.end()));
    ++_rows;
}
/**
 * @brief Adds row at bottom of Matrix
 * 
 * @param value containts of new row
 */
void Matrix::push_back_row(double value) {
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(empty())
        throw std::domain_error("Must have data to add row without size");
    std::vector<double> row(_columns, value);
    _data.push_back(row);
    ++_rows;
}
/**
 * @brief Adds blank row at bottom of Matrix
 * 
 */
void Matrix::push_back_row() {
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(empty())
        throw std::domain_error("Must have data to add row without size");
    std::vector<double> row(_columns);
    _data.push_back(row);
    ++_rows;
}

/**
 * @brief Adds column at right edge of Matrix
 * 
 * @param col list of doubles
 */
void Matrix::push_back_column(const std::initializer_list<double>& col) {
    if(_columns == MAX_MATRIX_SIZE)
        throw std::out_of_range("columns at max size");
    if(col.begin() == col.end())
        throw std::invalid_argument("Column cannot be empty");
    if(empty()) {
        _rows = col.size();
        for(ULL_int i=0; i<_rows; ++i)
            _data.push_back(std::vector<double>());
    } else if(_rows != col.size())
        throw 
            std::invalid_argument("Column must be same size as Matrix columns");
    ULL_int index = 0;
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
void Matrix::push_back_column(const std::vector<T>& col) {
    static_assert(std::is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_columns == MAX_MATRIX_SIZE)
        throw std::out_of_range("columns at max size");
    if(col.empty())
        throw std::invalid_argument("Column cannot be empty");
    if(empty()) {
        _rows = col.size();
        for(ULL_int i=0; i<_rows; ++i)
            _data.push_back(std::vector<double>());
    } else if(_rows != col.size())
        throw 
            std::invalid_argument("Column must be same size as Matrix columns");
    std::vector<double> colNew;
    ULL_int index = 0;
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
        throw std::out_of_range("columns at max size");
    if(empty())
        throw std::domain_error("Must have data to add column without size");
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
        throw std::out_of_range("columns at max size");
    if(empty())
        throw std::domain_error("Must have data to add column without size");
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
void Matrix::set_row(ULL_int row, const std::initializer_list<double>& rowNew) {
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw std::invalid_argument("Row must be same size as Matrix rows");
    _data[row] = rowNew;
}
/**
 * @brief Replace row of Matrix at given index
 * 
 * @param row row index of Matrix
 * @param rowNew new row (vector<int>)
 */
template <typename T>
void Matrix::set_row(ULL_int row, const std::vector<T>& rowNew) {
    static_assert(std::is_arithmetic<T>::value, "Vector must be arithmetic");
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw std::invalid_argument("Row must be same size as Matrix rows");
    _data[row] = std::vector<double>(rowNew.begin(), rowNew.end());
}
/**
 * @brief Replace row of Matrix at given index
 * 
 * @param row row index of Matrix
 * @param value values in new row
 */
void Matrix::set_row(ULL_int row, double value) {
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    for(ULL_int i=0; i<_columns; ++i) {
        _data[row][i] = value;
    }
}
/**
 * @brief Replace row of Matrix with 0's at given index
 * 
 * @param row row index of Matrix
 */
void Matrix::set_row(ULL_int row) {
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    for(ULL_int i=0; i<_columns; ++i) {
        _data[row][i] = 0;
    }
}

/**
 * @brief Replace column of Matrix at given index
 * 
 * @param col column index of Matrix
 * @param colNew new column (list)
 */
void Matrix::set_column(ULL_int col,
                        const std::initializer_list<double>& colNew){
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            std::invalid_argument("Column must be same size as Matrix columns");
    ULL_int index = 0;
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
void Matrix::set_column(ULL_int col, const std::vector<T>& colNew){
    static_assert(std::is_arithmetic<T>::value, "Vector must be arithmetic");
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            std::invalid_argument("Column must be same size as Matrix columns");
    ULL_int index = 0;
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
void Matrix::set_column(ULL_int col, double value) {
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    for(ULL_int i=0; i<_rows; ++i) {
        _data[i][col] = value;
    }
}
/**
 * @brief Replace column of Matrix with 0's at given index
 * 
 * @param col column index of Matrix
 */
void Matrix::set_column(ULL_int col) {
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    for(ULL_int i=0; i<_rows; ++i) {
        _data[i][col] = 0;
    }
}

/**
 * @brief Insert row at index and shift rows below
 * 
 * @param row index of Matrix to insert row
 * @param rowNew new row (list)
 */
void Matrix::insert_row(ULL_int row,
                        const std::initializer_list<double>& rowNew) {
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw std::invalid_argument("Row must be same size as Matrix rows");
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
void Matrix::insert_row(ULL_int row, const std::vector<T>& rowNew) {
    static_assert(std::is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    if(_columns != rowNew.size())
        throw std::invalid_argument("Row must be same size as Matrix rows");
    _data.insert(_data.begin() + row, 
                 std::vector<double>(rowNew.begin(), rowNew.end()));
    ++_rows;
}
/**
 * @brief Insert row of values at index and shift rows below
 * 
 * @param row index of Matrix to insert row
 * @param value value to fill row
 */
void Matrix::insert_row(ULL_int row, double value) {
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    _data.insert(_data.begin() + row, std::vector<double>(_columns, value));
    ++_rows;
}
/**
 * @brief Insert row of 0s at index and shift rows below
 * 
 * @param row index of Matrix to insert row
 */
void Matrix::insert_row(ULL_int row) {
    if(_rows == MAX_MATRIX_SIZE)
        throw std::out_of_range("rows at max size");
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
    _data.insert(_data.begin() + row, std::vector<double>(_columns));
    ++_rows;
}

/**
 * @brief Insert column at index and shift columns on the right
 * 
 * @param col index of Matrix to insert column
 * @param colNew new column (list)
 */
void Matrix::insert_column(ULL_int col, 
                           const std::initializer_list<double>& colNew) {
    if(_columns == MAX_MATRIX_SIZE)
        throw std::out_of_range("columns at max size");
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            std::invalid_argument("Column must be same size as Matrix columns");
    ULL_int index = 0;
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
void Matrix::insert_column(ULL_int col, const std::vector<T>& colNew) {
    static_assert(std::is_arithmetic<T>::value, "Vector must be arithmetic");
    if(_columns == MAX_MATRIX_SIZE)
        throw std::out_of_range("columns at max size");
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    if(_rows != colNew.size())
        throw 
            std::invalid_argument("Column must be same size as Matrix columns");
    ULL_int index = 0;
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
void Matrix::insert_column(ULL_int col, double value) {
    if(_columns == MAX_MATRIX_SIZE)
        throw std::out_of_range("columns at max size");
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    ULL_int index = 0;
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
void Matrix::insert_column(ULL_int col) {
    if(_columns == MAX_MATRIX_SIZE)
        throw std::out_of_range("columns at max size");
    if(col >= _columns)
        throw std::out_of_range("Column does not exist");
    ULL_int index = 0;
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
void Matrix::swap_row(ULL_int r1, ULL_int r2) {
    if(r1 >= _rows || r2 >= _rows)
        throw std::out_of_range("Row does not exist");
    std::vector<double> tmp = _data[r1];
    _data[r1] = _data[r2];
    _data[r2] = tmp;
}
/**
 * @brief Swaps 2 columns of given indexes of Matrix
 * 
 * 
 */
void Matrix::swap_column(ULL_int c1, ULL_int c2) {
    if(c1 >= _columns || c2 >= _columns)
        throw std::out_of_range("Column does not exist");
    std::vector<double> tmp;
    for(ULL_int i=0; i<_rows; ++i) {
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
        throw std::domain_error("No values to pop");
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
        throw std::domain_error("No values to pop");
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
void Matrix::erase_row(ULL_int row) {
    if(empty())
        throw std::domain_error("No values to erase");
    if(row >= _rows)
        throw std::out_of_range("Row does not exist");
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
void Matrix::erase_column(ULL_int col) {
    if(empty())
        throw std::domain_error("No values to erase");
    if(col >= _columns)
        throw std::out_of_range("Row does not exist");
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
 * @brief set Matrix to be the same as other (does not change float lenght)
 * 
 */
void Matrix::operator=(const Matrix& other) {
    _columns = other._columns;
    _rows = other._rows;
    _data = other._data;
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
        throw std::domain_error("Matricies must have data");
    if(other._columns != _columns || other._rows != _rows)
        throw std::invalid_argument
            ("Matricies must have same dimentions for addition");
    Matrix sum;
    for(ULL_int i=0; i<_rows; ++i) {
        std::vector<double> rowNew;
        for(ULL_int j=0; j<_columns; ++j) {
            rowNew.push_back(_data[i][j] + other._data[i][j]);
        }
        sum.push_back_row(rowNew);
    }
    return sum;
}

/**
 * @brief Multiply Matricices
 * 
 */
Matrix Matrix::operator*(const Matrix& other) const {
    if(empty() || other.empty())
        throw std::domain_error("Matricies must have data");
    if(_columns != other._rows)
        throw std::invalid_argument
            ("Invalid Matrix dimentions for multiplication");
    Matrix product;
    for(ULL_int i=0; i<_rows; ++i) {
        std::vector<double> rowNew;
        for(ULL_int j=0; j<other._columns; ++j) {
            double sum = 0;
            for(ULL_int k=0; k<_columns; ++k) {
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
        throw std::domain_error("Matrix must have data");
    Matrix product;
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        std::vector<double> rowNew;
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
        throw std::domain_error("Matrix must have data");
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        for(auto iter = rowIter->begin(); iter != rowIter->end(); ++iter) {
            *iter *= scale;
        }
    }
    return *this;
}

/**
 * @brief multiply Matrix with vector
 * 
 */
std::vector<double> Matrix::operator*(const std::vector<double>& vector) const {
    if(empty())
        throw std::domain_error("Matrix must have data");
    if(_columns != vector.size())
        throw std::invalid_argument
            ("Vector must be same size as number of columns");
    std::vector<double> product;
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        double sum = 0;
        ULL_int index = 0;
        for(auto iter = vector.begin(); iter != vector.end(); ++iter) {
            sum += *iter * rowIter->at(index++);
        }
        product.push_back(sum);
    }
    return product;
}
/**
 * @brief multiply Matrix with vector
 * 
 */
std::vector<double> Matrix::operator*(const std::vector<int>& vector) const {
    if(empty())
        throw std::domain_error("Matrix must have data");
    if(_columns != vector.size())
        throw std::invalid_argument
            ("Vector must be same size as number of columns");
    std::vector<double> product;
    for(auto rowIter = _data.begin(); rowIter != _data.end(); ++rowIter) {
        double sum = 0;
        ULL_int index = 0;
        for(auto iter = vector.begin(); iter != vector.end(); ++iter) {
            sum += *iter * rowIter->at(index++);
        }
        product.push_back(sum);
    }
    return product;
}
/**
 * @brief multiply Matrix with vector
 * 
 */
std::vector<double>
operator*(const std::vector<double>& vector, const Matrix& rhs) {
    return rhs.transpose() * vector;
}
/**
 * @brief multiply Matrix with vector
 * 
 */
std::vector<double>
operator*(const std::vector<int>& vector, const Matrix& rhs) {
    return rhs.transpose() * vector;
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
        throw std::invalid_argument("Matrix must have data");
    if(_columns != _rows)
        throw std::invalid_argument("Matrix must be square");
    Matrix M(_data);
    double scale = 1;
    for(ULL_int i=0; i<_columns; ++i) {
        ULL_int row = i; // starting row == number of past loops
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
                for(ULL_int j=i; j<_columns; ++j) {
                    M._data[row][j] -= coeff * M._data[i][j];
                }
            }
        }
    }
    for(ULL_int i=0; i<_rows; ++i) { // multiply diagonal
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
        throw std::invalid_argument("Matrix cannot be empty");
    Matrix M;
    for(ULL_int i = 0; i<_columns; ++i) {
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
        throw std::invalid_argument("Matrix cannot be empty");
    Matrix M = *this;
    ULL_int lead = 0; // column of current leading value
    for(ULL_int i=0; i<_rows && lead<_columns; ++i) {
        ULL_int row = i; // current row (start at top of unchanged lead values)
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
        std::swap(M._data[row], M._data[i]);
        double *leadingVals = new double[_rows]; // leading values at col: lead
        for(ULL_int k=0; k<_rows; ++k) {
            leadingVals[k] = M._data[k][lead];
        }
        for(ULL_int j=lead; j<_columns; ++j) {
            M._data[i][j] /= leadingVals[i];
            double tmp = M._data[i][j];
            for(ULL_int k=0; k<_rows; ++k) {
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
        throw std::invalid_argument("Matrix must have data");
    if(_columns != _rows)
        throw std::invalid_argument("Matrix must be square");
    if(_is_double_sub_zero(determinant())) {
        std::cerr << "Matrix not invertable";
        return Matrix();
    }
    Matrix M(_data);
    ULL_int len = _rows;
    for(ULL_int i=0; i<len; ++i) {
        std::vector<double> idCol(len);
        idCol[i] = 1;
        M.push_back_column(idCol);
    }
    Matrix I = M.rref();
    for(ULL_int i=0; i<len; ++i) {
        I.erase_column(0);
    }
    return I;
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
std::ostream& operator<<(std::ostream &os, const Matrix& mat) {
    /* Find max integer lenghts of each column */
    int *intMaxLen = new int[mat._columns]; // set to 1s
    for(ULL_int i=0; i<mat._columns; ++i) {
        intMaxLen[i] = 1;
    }
    bool allInt = true;
    for(std::vector<double> row : mat._data) {
        for(ULL_int i=0; i<mat._columns; ++i) {
            double intPart, floatPart = modf(row[i], &intPart);
            int intLen = log10(fabs(intPart) + mat._floatPrecis) + 1
                         + (intPart + mat._floatPrecis < 0); // +1 if negative
            if(intLen > intMaxLen[i]) {
                intMaxLen[i] = intLen;
            }
            if(fabs(floatPart + mat._floatPrecis) > 2 * mat._floatPrecis) {
                allInt = false;
            }
        }
    }
    int float_length = 0;
    if(!allInt) {
        float_length = mat._floatLen;
    }
    os << std::fixed << std::setprecision(float_length);
    for(std::vector<double> row : mat._data) {
        os << "|  ";
        for(ULL_int i=0; i<mat._columns; ++i) {
            os << std::setw(intMaxLen[i] + float_length + (float_length != 0)); 
            if(_is_double_sub_zero(row[i]))
                os << std::setprecision(0) << 0 
                   << std::setprecision(float_length) << "  ";
            else
                os << row[i] << "  ";
        }
        os << "|" << std::endl;
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
        throw std::invalid_argument
            ("float length must be less than MAX_FLOAT_LEN");
    _floatLen = len;
    _floatPrecis = pow(10, -(_floatLen + 1));
        
}

#pragma endregion // OUTPUT
/******************************************************************************/
#pragma region EXPLICIT_INSTANTIATIONS


/* int */
template Matrix::Matrix(const std::vector<std::vector<int>>& in);
template Matrix::Matrix(const std::vector<int>& in, orientation type);
template void Matrix::push_back_row(const std::vector<int>& row);
template void Matrix::push_back_column(const std::vector<int>& col);
template void Matrix::set_row(ULL_int row, const std::vector<int>& rowNew);
template void Matrix::set_column(ULL_int col, const std::vector<int>& colNew);
template void Matrix::insert_row(ULL_int row, const std::vector<int>& rowNew);
template void Matrix::insert_column(ULL_int col, const std::vector<int>& colNew);


/* double */
template Matrix::Matrix(const std::vector<std::vector<double>>& in);
template Matrix::Matrix(const std::vector<double>& in, orientation type);
template void Matrix::push_back_row(const std::vector<double>& row);
template void Matrix::push_back_column(const std::vector<double>& col);
template void Matrix::set_row(ULL_int row, const std::vector<double>& rowNew);
template void Matrix::set_column(ULL_int col, const std::vector<double>& colNew);
template void Matrix::insert_row(ULL_int row, const std::vector<double>& rowNew);
template void Matrix::insert_column(ULL_int col, const std::vector<double>& colNew);


/* float */
template Matrix::Matrix(const std::vector<std::vector<float>>& in);
template Matrix::Matrix(const std::vector<float>& in, orientation type);
template void Matrix::push_back_row(const std::vector<float>& row);
template void Matrix::push_back_column(const std::vector<float>& col);
template void Matrix::set_row(ULL_int row, const std::vector<float>& rowNew);
template void Matrix::set_column(ULL_int col, const std::vector<float>& colNew);
template void Matrix::insert_row(ULL_int row, const std::vector<float>& rowNew);
template void Matrix::insert_column(ULL_int col, const std::vector<float>& colNew);

#pragma endregion // EXPLICIT_INSTANTIATIONS
