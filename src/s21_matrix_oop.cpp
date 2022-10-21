#include "s21_matrix_oop.hpp"

S21Matrix::S21Matrix()
    : rows_(16), cols_(16), matrix_(new double[rows_ * cols_]()) {
}

S21Matrix::S21Matrix(int32_t rows, int32_t cols) : rows_(rows), cols_(cols) {
    if (rows_ <= 0 || cols_ <= 0)
        throw std::length_error("Array size can't be zero");

    matrix_ = new double[rows_ * cols_]();
}

S21Matrix::~S21Matrix() {
    delete[] matrix_;
}

S21Matrix::S21Matrix(const S21Matrix &other)
    : rows_(other.rows_), cols_(other.cols_),
      matrix_(new double[rows_ * cols_]()) {

    std::copy(other.matrix_, other.matrix_ + rows_ * cols_, matrix_);
}

S21Matrix::S21Matrix(S21Matrix &&other) noexcept {
    rows_ = std::exchange(other.rows_, 0);
    cols_ = std::exchange(other.cols_, 0);
    matrix_ = std::exchange(other.matrix_, nullptr);
}

// The rule of four and a half https://www.youtube.com/watch?v=7Qgd9B1KuMQ
// Only one assignment operation implementation for move and copy
// START
S21Matrix &S21Matrix::operator=(S21Matrix copy) noexcept {
    copy.swap(*this);
    return *this;
}

void S21Matrix::swap(S21Matrix &rhs) noexcept {
    using std::swap;
    swap(matrix_, rhs.matrix_);
    swap(rows_, rhs.rows_);
    swap(cols_, rhs.cols_);
}

void swap(S21Matrix &a, S21Matrix &b) noexcept {
    a.swap(b);
}
// END

int32_t S21Matrix::get_rows() const {
    return rows_;
}

int32_t S21Matrix::get_cols() const {
    return cols_;
}

double &S21Matrix::operator()(int32_t row, int32_t col) {
    if (row >= rows_ || col >= cols_)
        throw std::out_of_range("Incorrect input, index is out of range");

    return matrix_[row * cols_ + col];
}

double *S21Matrix::operator[](int32_t row) const {
    if (row >= rows_)
        throw std::out_of_range("Incorrect input, index is out of range");

    return row * cols_ + matrix_;
}

// S21Matrix &S21Matrix::operator=(S21Matrix &&other) noexcept {
//     if (this == &other)
//         return *this;
//
//     std::swap(rows_, other.rows_);
//     std::swap(cols_, other.cols_);
//     std::swap(matrix_, other.matrix_);
//
//     return *this;
// }

void S21Matrix::set_rows(const int32_t &new_rows) {
    if (new_rows <= 0)
        throw std::length_error("Array size can't be zero");

    S21Matrix tmp(new_rows, cols_);
    for (int32_t i = 0; i < (rows_ < new_rows ? rows_ : new_rows); ++i)
        for (int32_t j = 0; j < cols_; ++j)
            tmp[i][j] = (*this)[i][j];

    *this = std::move(tmp);
}

void S21Matrix::set_cols(const int32_t &new_cols) {
    if (new_cols <= 0)
        throw std::length_error("Array size can't be zero");

    S21Matrix tmp(rows_, new_cols);
    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < (cols_ < new_cols ? cols_ : new_cols); ++j)
            tmp[i][j] = (*this)[i][j];

    *this = std::move(tmp);
}

// S21Matrix &S21Matrix::operator=(const S21Matrix &other) noexcept {
//     if (this != &other) {
//         delete[] matrix_;
//
//         rows_ = other.rows_;
//         cols_ = other.cols_;
//
//         matrix_ = new double[rows_ * cols_]();
//         std::copy(other.matrix_, other.matrix_ + rows_ * cols_, matrix_);
//     }
//     return *this;
// }

bool S21Matrix::operator==(const S21Matrix &other) {
    return EqMatrix(other);
}

bool S21Matrix::EqMatrix(const S21Matrix &other) const {
    if (rows_ != other.get_rows() || cols_ != other.get_cols())
        return false;

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            if (std::fabs((*this)[i][j] - other[i][j]) > 1e-07)
                return false;

    return true;
}

S21Matrix &S21Matrix::operator+=(const S21Matrix &other) {
    SumMatrix(other);
    return *this;
}

S21Matrix S21Matrix::operator+(const S21Matrix &other) const {
    if (rows_ != other.get_rows() || cols_ != other.get_cols())
        throw "Can't sum matrices of different dimensions";

    S21Matrix res(rows_, cols_);

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            res[i][j] = (*this)[i][j] + other[i][j];

    return res;
}

void S21Matrix::SumMatrix(const S21Matrix &other) {
    if (rows_ != other.get_rows() || cols_ != other.get_cols())
        throw "Can't sum matrices of different dimensions";

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            (*this)[i][j] += other[i][j];
}

S21Matrix &S21Matrix::operator-=(const S21Matrix &other) {
    SubMatrix(other);
    return *this;
}

S21Matrix S21Matrix::operator-(const S21Matrix &other) const {
    if (rows_ != other.get_rows() || cols_ != other.get_cols())
        throw "Can't subtract matrices of different dimensions";

    S21Matrix res(rows_, cols_);

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            res[i][j] = (*this)[i][j] - other[i][j];

    return res;
}

void S21Matrix::SubMatrix(const S21Matrix &other) {
    if (rows_ != other.get_rows() || cols_ != other.get_cols())
        throw "Can't subtract matrices of different dimensions";

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            (*this)[i][j] -= other[i][j];
}

S21Matrix &S21Matrix::operator*=(const S21Matrix &other) {
    MulMatrix(other);
    return *this;
}

S21Matrix &S21Matrix::operator*=(const double &value) {
    MulNumber(value);
    return *this;
}

S21Matrix S21Matrix::operator*(const S21Matrix &other) const {
    if (cols_ != other.get_rows() || rows_ != other.get_cols())
        throw "Dimensions don't fit for the multiplication";

    S21Matrix res(rows_, other.cols_);

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < other.get_cols(); ++j)
            for (int32_t k = 0; k < cols_; k++)
                res[i][j] += (*this)[i][k] * other[k][j];

    return res;
}

S21Matrix S21Matrix::operator*(const double &value) const {
    S21Matrix res(rows_, cols_);

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            res[i][j] = (*this)[i][j] * value;

    return res;
}

S21Matrix operator*(const double &value, const S21Matrix &matrix) {
    int32_t rows = matrix.get_rows();
    int32_t cols = matrix.get_cols();
    S21Matrix res(rows, cols);

    for (int32_t i = 0; i < rows; ++i)
        for (int32_t j = 0; j < cols; ++j)
            res[i][j] = matrix[i][j] * value;

    return res;
}

void S21Matrix::MulNumber(const double num) {
    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            (*this)[i][j] *= num;
}

void S21Matrix::MulMatrix(const S21Matrix &other) {
    if (cols_ != other.get_rows() || rows_ != other.get_cols())
        throw "Dimensions don't fit for the multiplication";

    S21Matrix res(this->rows_, other.get_cols());

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < other.get_cols(); ++j)
            for (int32_t k = 0; k < cols_; k++)
                res[i][j] += (*this)[i][k] * other[k][j];

    *this = std::move(res);
}

S21Matrix S21Matrix::Transpose() {
    S21Matrix res(cols_, rows_);

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            res[j][i] = (*this)[i][j];

    return res;
}

namespace {

void get_cofactor(const S21Matrix &m, S21Matrix &tmp, int32_t skip_row,
                  int32_t skip_col, int32_t size) {
    for (int32_t i = 0, row = 0; row < size; row++) {
        for (int32_t j = 0, col = 0; col < size; col++) {
            if (row != skip_row && col != skip_col) {
                tmp[i][j] = m[row][col];
                ++j;
                if (j == size - 1) {
                    j = 0;
                    ++i;
                }
            }
        }
    }
}

double det(const S21Matrix &m, int32_t size) {

    double res = 0;

    if (size == 1)
        return m[0][0];

    S21Matrix tmp(size, size);

    int sign = 1;
    for (int32_t i = 0; i < size; ++i) {
        get_cofactor(m, tmp, 0, i, size);
        res += sign * m[0][i] * det(tmp, size - 1);
        sign = -sign;
    }

    return res;
}

S21Matrix calc_complements(const S21Matrix &m) {
    const int32_t rows = m.get_rows();
    const int32_t cols = m.get_cols();

    if (rows == 1) {
        m[0][0] = 1;
        return m;
    }

    S21Matrix tmp(rows, cols);
    S21Matrix res(rows, cols);

    for (int32_t i = 0; i < rows; ++i) {
        for (int32_t j = 0; j < cols; ++j) {
            get_cofactor(m, tmp, i, j, rows);

            int sign = ((i + j) % 2 == 0) ? 1 : -1;

            res[i][j] = sign * det(tmp, rows - 1);
        }
    }
    return res;
}
}  // namespace

double S21Matrix::Determinant() {
    if (this->rows_ != this->cols_)
        throw "The matrix is not square to calculate determinant";

    return det(*this, rows_);
}

S21Matrix S21Matrix::CalcComplements() {
    if (this->rows_ != this->cols_)
        throw "The matrix is not square to calculate the complements";
    return calc_complements(*this);
}

S21Matrix S21Matrix::InverseMatrix() {
    double det = Determinant();
    if (rows_ != cols_)
        throw "The matrix is not square to calculate the inverse";

    if (std::fabs(det) < 1e-06)
        throw "Determinant can't be zero to calculate inverse";

    S21Matrix adj_transposed = CalcComplements().Transpose();
    S21Matrix res(rows_, cols_);

    for (int32_t i = 0; i < rows_; ++i)
        for (int32_t j = 0; j < cols_; ++j)
            res[i][j] = adj_transposed[i][j] / det;

    return res;
}
