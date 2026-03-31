#ifndef CSR_MATRIX_HPP
#define CSR_MATRIX_HPP

#include <vector>
#include <exception>

namespace sjtu {

class size_mismatch : public std::exception {
public:
    const char *what() const noexcept override {
        return "Size mismatch";
    }
};

class invalid_index : public std::exception {
public:
    const char *what() const noexcept override {
        return "Index out of range";
    }
};

// TODO: Implement a CSR matrix class
// You only need to implement the TODOs in this file
// DO NOT modify other parts of this file
// DO NOT include any additional headers
// DO NOT use STL other than std::vector

template <typename T>
class CSRMatrix {

private:
    // TODO: add your private member variables here
    size_t n_rows{};
    size_t n_cols{};
    std::vector<size_t> indptr_;
    std::vector<size_t> indices_;
    std::vector<T> data_;

    // binary search for column j in row i; returns position via pos, and bool found
    bool find_in_row(size_t i, size_t j, size_t &pos) const {
        size_t l = indptr_[i];
        size_t r = indptr_[i + 1];
        while (l < r) {
            size_t mid = l + ((r - l) >> 1);
            size_t col = indices_[mid];
            if (col == j) { pos = mid; return true; }
            if (col < j) l = mid + 1; else r = mid;
        }
        pos = l;
        return false;
    }

public:
    // Assignment operators are deleted
    CSRMatrix &operator=(const CSRMatrix &other) = delete;
    CSRMatrix &operator=(CSRMatrix &&other) = delete;

    // Constructor for empty matrix with dimensions
    // TODO: Initialize an empty CSR matrix with n rows and m columns
    CSRMatrix(size_t n, size_t m)
        : n_rows(n), n_cols(m), indptr_(n + 1, 0), indices_(), data_() {}

    // Constructor with pre-built CSR components
    // TODO: Initialize CSR matrix from existing CSR format data, validate sizes
    CSRMatrix(size_t n, size_t m, size_t count,
        const std::vector<size_t> &indptr,
        const std::vector<size_t> &indices,
        const std::vector<T> &data)
        : n_rows(n), n_cols(m), indptr_(indptr), indices_(indices), data_(data) {
        if (indptr_.size() != n_rows + 1) throw size_mismatch();
        if (indices_.size() != count || data_.size() != count) throw size_mismatch();
        if (indptr_.empty()) throw size_mismatch();
        if (indptr_.back() != count) throw size_mismatch();
        // validate non-decreasing indptr and indices range
        for (size_t i = 1; i < indptr_.size(); ++i) {
            if (indptr_[i] < indptr_[i - 1]) throw size_mismatch();
        }
        for (size_t k = 0; k < indices_.size(); ++k) {
            if (indices_[k] >= n_cols) throw invalid_index();
        }
        // optionally ensure each row's indices are sorted non-decreasing
        for (size_t i = 0; i < n_rows; ++i) {
            size_t s = indptr_[i], e = indptr_[i + 1];
            for (size_t k = s + 1; k < e; ++k) {
                if (indices_[k] < indices_[k - 1]) throw size_mismatch();
            }
        }
    }

    // Copy constructor
    CSRMatrix(const CSRMatrix &other) = default;

    // Move constructor
    CSRMatrix(CSRMatrix &&other) = default;

    // Constructor from dense matrix format (given as vector of vectors)
    // TODO: Convert dense matrix representation to CSR format
    CSRMatrix(size_t n, size_t m, const std::vector<std::vector<T>> &dense)
        : n_rows(n), n_cols(m), indptr_(n + 1, 0) {
        if (dense.size() != n_rows) throw size_mismatch();
        // pre-count to reserve
        size_t cnt = 0;
        for (size_t i = 0; i < n_rows; ++i) {
            if (dense[i].size() != n_cols) throw size_mismatch();
            for (size_t j = 0; j < n_cols; ++j) {
                if (!(dense[i][j] == T())) ++cnt;
            }
        }
        indices_.reserve(cnt);
        data_.reserve(cnt);
        size_t running = 0;
        for (size_t i = 0; i < n_rows; ++i) {
            for (size_t j = 0; j < n_cols; ++j) {
                const T &v = dense[i][j];
                if (!(v == T())) {
                    indices_.push_back(j);
                    data_.push_back(v);
                    ++running;
                }
            }
            indptr_[i + 1] = running;
        }
    }

    // Destructor
    ~CSRMatrix() = default;

    // Get dimensions and non-zero count
    // TODO: Return the number of rows
    size_t getRowSize() const { return n_rows; }

    // TODO: Return the number of columns
    size_t getColSize() const { return n_cols; }

    // TODO: Return the count of non-zero elements
    size_t getNonZeroCount() const { return data_.size(); }

    // Element access
    // TODO: Retrieve element at position (i,j)
    T get(size_t i, size_t j) const {
        if (i >= n_rows || j >= n_cols) throw invalid_index();
        size_t pos = 0; bool found = find_in_row(i, j, pos);
        if (found) return data_[pos];
        return T();
    }

    // TODO: Set element at position (i,j), updating CSR structure as needed
    void set(size_t i, size_t j, const T &value) {
        if (i >= n_rows || j >= n_cols) throw invalid_index();
        size_t pos = 0; bool found = find_in_row(i, j, pos);
        if (found) {
            data_[pos] = value;
            return;
        }
        // insert new entry at position pr.second
        indices_.insert(indices_.begin() + static_cast<long>(pos), j);
        data_.insert(data_.begin() + static_cast<long>(pos), value);
        // update indptr for subsequent rows
        for (size_t r = i + 1; r < indptr_.size(); ++r) ++indptr_[r];
    }

    // Access CSR components
    // TODO: Return the row pointer array
    const std::vector<size_t> &getIndptr() const { return indptr_; }

    // TODO: Return the column indices array
    const std::vector<size_t> &getIndices() const { return indices_; }

    // TODO: Return the data values array
    const std::vector<T> &getData() const { return data_; }

    // Convert to dense matrix format
    // TODO: Convert CSR format to dense matrix representation
    std::vector<std::vector<T>> getMatrix() const {
        std::vector<std::vector<T>> dense(n_rows, std::vector<T>(n_cols, T()));
        for (size_t i = 0; i < n_rows; ++i) {
            size_t s = indptr_[i], e = indptr_[i + 1];
            for (size_t k = s; k < e; ++k) {
                dense[i][indices_[k]] = data_[k];
            }
        }
        return dense;
    }

    // Matrix-vector multiplication
    // TODO: Implement multiplication of this matrix with vector vec
    std::vector<T> operator*(const std::vector<T> &vec) const {
        if (vec.size() != n_cols) throw size_mismatch();
        std::vector<T> res(n_rows, T());
        for (size_t i = 0; i < n_rows; ++i) {
            size_t s = indptr_[i], e = indptr_[i + 1];
            T sum = T();
            for (size_t k = s; k < e; ++k) {
                sum = sum + data_[k] * vec[indices_[k]];
            }
            res[i] = sum;
        }
        return res;
    }

    // Row slicing
    // TODO: Extract submatrix containing rows [l,r)
    CSRMatrix getRowSlice(size_t l, size_t r) const {
        if (l > r || r > n_rows) throw invalid_index();
        size_t new_n = r - l;
        size_t base = indptr_[l];
        size_t new_cnt = indptr_[r] - base;
        std::vector<size_t> new_indptr(new_n + 1, 0);
        for (size_t i = 0; i < new_n; ++i) {
            new_indptr[i + 1] = indptr_[l + i + 1] - base;
        }
        std::vector<size_t> new_indices;
        std::vector<T> new_data;
        new_indices.reserve(new_cnt);
        new_data.reserve(new_cnt);
        for (size_t k = base; k < indptr_[r]; ++k) {
            new_indices.push_back(indices_[k]);
            new_data.push_back(data_[k]);
        }
        return CSRMatrix(new_n, n_cols, new_cnt, new_indptr, new_indices, new_data);
    }
};

}

#endif // CSR_MATRIX_HPP
