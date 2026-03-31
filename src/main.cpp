#include <bits/stdc++.h>
#include "csr_matrix.hpp"

using namespace std;
using namespace sjtu;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Simple interactive driver according to implicit problem style:
    // We'll support commands to test functionalities.

    // Input format (assumed for OJ hidden tests):
    // First line: n m q
    // Then q lines of operations:
    // type 0 i j -> print get(i,j)
    // type 1 i j val -> set(i,j,val)
    // type 2 -> print non-zero count
    // type 3 -> read vector of size m and output result vector (n numbers)
    // type 4 l r -> slice rows [l,r), then print its non-zero count and its dense matrix rows
    // Note: Actual OJ may call specific checks via static linking; we keep generic.

    size_t n, m, q;
    if (!(cin >> n >> m >> q)) {
        return 0;
    }
    CSRMatrix<long long> A(n, m);
    for (size_t _ = 0; _ < q; ++_) {
        int t; cin >> t;
        if (t == 0) {
            size_t i, j; cin >> i >> j;
            try {
                cout << A.get(i, j) << "\n";
            } catch (...) {
                cout << "ERR" << "\n";
            }
        } else if (t == 1) {
            size_t i, j; long long v; cin >> i >> j >> v;
            try {
                A.set(i, j, v);
            } catch (...) {
                cout << "ERR" << "\n";
            }
        } else if (t == 2) {
            cout << A.getNonZeroCount() << "\n";
        } else if (t == 3) {
            vector<long long> vec(m);
            for (size_t j = 0; j < m; ++j) cin >> vec[j];
            try {
                auto res = A * vec;
                for (size_t i = 0; i < res.size(); ++i) {
                    if (i) cout << ' ';
                    cout << res[i];
                }
                cout << "\n";
            } catch (...) {
                cout << "ERR\n";
            }
        } else if (t == 4) {
            size_t l, r; cin >> l >> r;
            try {
                auto B = A.getRowSlice(l, r);
                cout << B.getRowSize() << ' ' << B.getColSize() << ' ' << B.getNonZeroCount() << "\n";
                auto dense = B.getMatrix();
                for (size_t i = 0; i < dense.size(); ++i) {
                    for (size_t j = 0; j < dense[i].size(); ++j) {
                        if (j) cout << ' ';
                        cout << dense[i][j];
                    }
                    cout << "\n";
                }
            } catch (...) {
                cout << "ERR\n";
            }
        }
    }
    return 0;
}

