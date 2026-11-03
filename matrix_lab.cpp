#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

// The Matrix class (Practice 1)
class Matrix {
private:
    int  n_;     // dimension (matrix is N x N)
    int* data_;  // the N*N elements, row-major

    // Internal unchecked access (bounds already validated by caller)
    int& at(int row, int col)       { return data_[row * n_ + col]; }
    int  at(int row, int col) const { return data_[row * n_ + col]; }

    void checkIndex(int row, int col) const {
        if (row < 0 || row >= n_ || col < 0 || col >= n_)
            throw std::out_of_range("Matrix index out of range");
    }

public:
    Matrix() : n_(0), data_(nullptr) {}

    explicit Matrix(int n) : n_(0), data_(nullptr) {
        if (n <= 0) throw std::invalid_argument("Matrix size must be positive");
        n_     = n;
        data_  = new int[n_ * n_]();   // zero-initialized
    }

    // Rule of Three — the class owns raw memory, so we need
    // a proper copy constructor, copy assignment, and destructor.
    Matrix(const Matrix& other)
        : n_(other.n_), data_(new int[other.n_ * other.n_]) {
        std::copy(other.data_, other.data_ + n_ * n_, data_);
    }

    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            int* fresh = new int[other.n_ * other.n_];
            std::copy(other.data_, other.data_ + other.n_ * other.n_, fresh);
            delete[] data_;
            data_ = fresh;
            n_    = other.n_;
        }
        return *this;
    }

    ~Matrix() { delete[] data_; }

    int size() const { return n_; }

    int get(int row, int col) const {
        checkIndex(row, col);
        return at(row, col);
    }

    void set(int row, int col, int value) {
        checkIndex(row, col);
        at(row, col) = value;
    }

    // Nicely aligned printing: fixed-width right-aligned columns
    void display(std::ostream& os = std::cout) const {
        for (int r = 0; r < n_; ++r) {
            for (int c = 0; c < n_; ++c)
                os << std::setw(6) << at(r, c);
            os << '\n';
        }
    }

    // Practice 1: read a user-specified file and initialize.
    // Format: line 1 = N, then the data for two N x N matrices.
    static bool readFromFile(const std::string& filename, Matrix& a, Matrix& b) {
        std::ifstream in(filename);
        if (!in) return false;

        int n = 0;
        if (!(in >> n) || n <= 0) return false;

        Matrix loaded[2] = { Matrix(n), Matrix(n) };
        for (int which = 0; which < 2; ++which) {
            for (int r = 0; r < n; ++r) {
                for (int c = 0; c < n; ++c) {
                    int v = 0;
                    if (!(in >> v)) return false;
                    loaded[which].set(r, c, v);
                }
            }
        }
        a = loaded[0];
        b = loaded[1];
        return true;
    }

    // Practice 2: operator overloading for addition
    Matrix operator+(const Matrix& rhs) const {
        if (n_ != rhs.n_)
            throw std::invalid_argument("Matrix sizes do not match");
        Matrix result(n_);
        for (int i = 0; i < n_ * n_; ++i)
            result.data_[i] = data_[i] + rhs.data_[i];
        return result;
    }

    // Practice 3: operator overloading for multiplication
    Matrix operator*(const Matrix& rhs) const {
        if (n_ != rhs.n_)
            throw std::invalid_argument("Matrix sizes do not match");
        Matrix result(n_);
        for (int r = 0; r < n_; ++r) {
            for (int k = 0; k < n_; ++k) {
                const int factor = at(r, k);
                if (factor == 0) continue;                 // small speed-up
                for (int c = 0; c < n_; ++c)
                    result.at(r, c) += factor * rhs.at(k, c);
            }
        }
        return result;
    }

    // Practice 4: diagonal sums
    int mainDiagonalSum() const {
        int sum = 0;
        for (int i = 0; i < n_; ++i) sum += at(i, i);
        return sum;
    }

    int secondaryDiagonalSum() const {
        int sum = 0;
        for (int i = 0; i < n_; ++i) sum += at(i, n_ - 1 - i);
        return sum;
    }

    // Low-level swaps used by the pass-by-value helpers below.
    void swapRows(int r1, int r2) {
        if (r1 == r2) return;
        checkIndex(r1, 0);
        checkIndex(r2, 0);
        std::swap_ranges(data_ + r1 * n_, data_ + (r1 + 1) * n_,
                         data_ + r2 * n_);
    }

    void swapColumns(int c1, int c2) {
        if (c1 == c2) return;
        checkIndex(0, c1);
        checkIndex(0, c2);
        for (int r = 0; r < n_; ++r)
            std::swap(at(r, c1), at(r, c2));
    }
};

// Practice 5: swap rows — the Matrix is passed BY VALUE,
// so the caller's original matrix is never modified.
// Default indices: 0 and 1 (per the lab sheet).
void swapRowsAndDisplay(Matrix m, int r1 = 0, int r2 = 1) {
    if (r1 < 0 || r1 >= m.size() || r2 < 0 || r2 >= m.size()) {
        std::cout << "Invalid row indices (" << r1 << ", " << r2 << ").\n";
        return;
    }
    m.swapRows(r1, r2);
    m.display();
}

// Practice 6: swap columns — also pass by value.
// Default indices: 0 and 1.
void swapColumnsAndDisplay(Matrix m, int c1 = 0, int c2 = 1) {
    if (c1 < 0 || c1 >= m.size() || c2 < 0 || c2 >= m.size()) {
        std::cout << "Invalid column indices (" << c1 << ", " << c2 << ").\n";
        return;
    }
    m.swapColumns(c1, c2);
    m.display();
}


// Practice 7: update one element — pass by value again.
// Defaults: row 0, col 0, value 100 (per the lab sheet).
void updateAndDisplay(Matrix m, int row = 0, int col = 0, int value = 100) {
    if (row < 0 || row >= m.size() || col < 0 || col >= m.size()) {
        std::cout << "Invalid indices (row=" << row << ", col=" << col << ").\n";
        return;
    }
    m.set(row, col, value);
    m.display();
}

int main(int argc, char* argv[]) {
    const std::string filename = (argc > 1) ? argv[1] : "input.txt";

    Matrix a, b;
    try {
        if (!Matrix::readFromFile(filename, a, b)) {
            std::cerr << "Could not read matrix data from \"" << filename << "\".\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error while reading input: " << e.what() << '\n';
        return 1;
    }

    // Practice 1: display both matrices, neatly aligned
    std::cout << "===== Matrix A =====\n";
    a.display();
    std::cout << "\n===== Matrix B =====\n";
    b.display();

    // Practice 2: addition via operator overloading
    std::cout << "\n===== A + B =====\n";
    Matrix sum = a + b;
    sum.display();

    // Practice 3: multiplication via operator overloading
    std::cout << "\n===== A * B =====\n";
    Matrix product = a * b;
    product.display();

    // Practice 4: diagonal sums
    std::cout << "\nMain diagonal sum of A:      " << a.mainDiagonalSum() << '\n';
    std::cout << "Secondary diagonal sum of A: " << a.secondaryDiagonalSum() << '\n';

    // Practice 5: swap rows (pass by value, default indices 0 and 1)
    std::cout << "\n===== A after swapping rows 0 and 1 =====\n";
    swapRowsAndDisplay(a);
    std::cout << "\n===== A after swapping rows 1 and 3 =====\n";
    swapRowsAndDisplay(a, 1, 3);

    // Proof the original is untouched (because of pass-by-value)
    std::cout << "\n===== Original A (unchanged) =====\n";
    a.display();

    // Practice 6: swap columns (pass by value, default indices 0 and 1)
    std::cout << "\n===== A after swapping columns 0 and 1 =====\n";
    swapColumnsAndDisplay(a);
    std::cout << "\n===== A after swapping columns 2 and 3 =====\n";
    swapColumnsAndDisplay(a, 2, 3);

    // Practice 7: update an element (defaults: row 0, col 0, value 100)
    std::cout << "\n===== A with element (0,0) set to 100 =====\n";
    updateAndDisplay(a);
    std::cout << "\n===== A with element (2,3) set to 999 =====\n";
    updateAndDisplay(a, 2, 3, 999);

    return 0;
}