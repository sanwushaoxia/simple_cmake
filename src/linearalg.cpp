#include <vector>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? (-a) : (a))

using namespace std;

/* 计算逆序数的笨方法 */
long long inverse_order_number_stupid(vector<int> &v)
{
    long long ret = 0;
    for (int i = 0; i < v.size(); ++i) {
        for (int j = i + 1; j < v.size(); ++j) {
            if (v[i] > v[j]) {
                ++ret;
            }
        }
    }
    return ret;
}

/* 递归地计算逆序数的方法 */
long long inverse_order_number_recursion(vector<int> &v, int beg, int end, vector<int> &tmp)
{
    if (end - beg > 1) {
        int mid = beg + (end - beg) / 2;
        long long ret = inverse_order_number_recursion(v, beg, mid, tmp) + inverse_order_number_recursion(v, mid, end, tmp);
        int i = beg;
        int j = mid;
        int l = beg;
        while (i < mid && j < end) {
            if (v[i] > v[j]) {
                tmp[l++] = v[j++];
            } else {
                ret += (j - mid);
                tmp[l++] = v[i++];
            }
        }
        while (i < mid) {
            ret += (j - mid); // j - mid == end - mid
            tmp[l++] = v[i++];
        }
        while (j < end) {
            tmp[l++] = v[j++];
        }
        for (int k = beg; k < end; ++k) {
            v[k] = tmp[k];
        }
        return ret;
    } else {
        return 0;
    }
}

inline long long merge(vector<int> &v, int left, int right, int l, vector<int> &tmp)
{
    int ret = 0;
    int i = left;
    int j = right;
    int k = left;
    while (i < right && j < right + l) {
        if (v[i] > v[j]) {
            tmp[k++] = v[j++];
        } else {
            ret += (j - right);
            tmp[k++] = v[i++];
        }
    }
    while (i < right) {
        ret += (j - right);
        tmp[k++] = v[i++];
    }
    while (j < right + l) {
        tmp[k++] = v[j++];
    }
    for (int k = left; k < right + l; ++k) {
        v[k] = tmp[k];
    }
    return ret;
}

/* 迭代地计算逆序数的方法 */
long long inverse_order_number(vector<int> &v)
{
    int len = v.size();
    int ret = 0;
    vector<int> tmp(len, 0);

    for (int l = 1; l < len; l *= 2) {
        int left = 0;
        int right;
        while (left < len) {
            right = left + l;
            if (right >= len) {
                break;
            }
            ret += merge(v, left, right, MIN(l, len - right), tmp);
            left = right + l;
        }
    }

    return ret;
}

/* 使用不同方法计算逆序数, 并比较其性能 */
int cal_inverse_order()
{
    vector<int> v;
    srand(time(0));
    for (int i = 0; i < 1e5; ++i) {
        v.emplace_back(rand());
    }
    vector<int> v_copy(v);
    struct timespec s = {0}, e = {0};
    long long inv;
    double diff;
    double N;

    cout << "inverse order of v ";
    clock_gettime(CLOCK_MONOTONIC, &s);
    inv = inverse_order_number_stupid(v);
    clock_gettime(CLOCK_MONOTONIC, &e);
    diff = (e.tv_sec - s.tv_sec) + (e.tv_nsec - s.tv_nsec) / 1e9;
    cout << "is " << inv << endl;
    N = v.size() * v.size();
    cout << "stupid spend " << diff << " time, " << "per oper = " << diff / N << endl;

    cout << "inverse order of v ";
    vector<int> tmp;
    tmp.reserve(v.size());
    clock_gettime(CLOCK_MONOTONIC, &s);
    inv = inverse_order_number_recursion(v_copy, 0, v_copy.size(), tmp);
    clock_gettime(CLOCK_MONOTONIC, &e);
    diff = (e.tv_sec - s.tv_sec) + (e.tv_nsec - s.tv_nsec) / 1e9;
    cout << "is " << inv << endl;
    N = v.size() * log2(v.size());
    cout << "recursion spend " << diff << " time, " << "per oper = " << diff / N << endl;

    cout << "inverse order of v ";
    clock_gettime(CLOCK_MONOTONIC, &s);
    inv = inverse_order_number(v);
    clock_gettime(CLOCK_MONOTONIC, &e);
    diff = (e.tv_sec - s.tv_sec) + (e.tv_nsec - s.tv_nsec) / 1e9;
    cout << "is " << inv << endl;
    N = v.size() * log2(v.size());
    cout << "spend " << diff << " time, " << "per oper = " << diff / N << endl;

    return 0;
}



/* 矩阵的初等变换 */
void swap_rows(vector<vector<double>> &mat, int i, int j)
{
    double tmp;
    for (int k = 0; k < mat[0].size(); ++k) {
        tmp = mat[i][k];
        mat[i][k] = mat[j][k];
        mat[j][k] = tmp;
    }
    return;
}

void swap_cols(vector<vector<double>> &mat, int i, int j)
{
    double tmp;
    for (int k = 0; k < mat.size(); ++k) {
        tmp = mat[k][i];
        mat[k][i] = mat[k][j];
        mat[k][j] = tmp;
    }
    return;
}

void multiply_row(vector<vector<double>> &mat, int i, double k)
{
    for (int k = 0; k < mat[0].size(); ++k) {
        mat[i][k] *= k;
    }
    return;
}

void add_row_multiple(vector<vector<double>> &mat, int i, int j, double k)
{
    for (int l = 0; l < mat[0].size(); ++l) {
        mat[i][l] += k * mat[j][l];
    }
    return;
}

void print_matrix(vector<vector<double>> &mat)
{
    for (int i = 0; i < mat.size(); ++i) {
        for (int j = 0; j < mat[0].size(); ++j) {
            cout << fixed << setprecision(4) << mat[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

int convert_matrix_to_stepped(vector<vector<double>> &mat)
{
    int co = 1;
    for (int j = 0; j < mat[0].size(); ++j) {
        // cout << j << endl;
        int max_idx = -1;
        int max_val = 0;
        for (int i = j; i < mat.size(); ++i) {
            if (ABS(mat[i][j]) > max_val) {
                max_idx = i;
                max_val = mat[i][j];
            }
        }

        if (max_idx == -1) {
            continue;
        }

        if (j != max_idx) {
            swap_rows(mat, j, max_idx);
            co *= -1;
            // cout << "coefficient = " << co << endl;
            // print_matrix(mat);
        }

        double k;
        for (int i = j + 1; i < mat.size(); ++i) {
            k = -mat[i][j] / mat[j][j];
            add_row_multiple(mat, i, j, k);
            // print_matrix(mat);
        }
    }
    return co;
}

double cal_det(vector<vector<double>> &mat, int co)
{
    double prod = 1;
    for (int j = 0; j < MIN(mat.size(), mat[0].size()); ++j) {
        prod *= mat[j][j];
    }
    cout << "multiple = " << co * prod << endl;
    return co * prod;
}

/* 通过线性方程组的增广矩阵求其唯一解 */
int solve_linear_equations()
{
    
    // vector<vector<double>> mat{{1, 2, 3},
    //                            {4, 5, 6},
    //                            {7, 8, 10}};
    // vector<vector<double>> mat{{1, 2, 3},
    //                            {4, 5, 6},
    //                            {7, 8, 11}};
    vector<vector<double>> mat{{1, 1, 1, 1, 10},
                               {2, 1, -1, 1, 5},
                               {1, -1, 1, 2, 10},
                               {1, 2, 1, -1, 4}};
    vector<vector<double>> mat_copy(mat);

    int co = convert_matrix_to_stepped(mat_copy);
    print_matrix(mat_copy);
    double d = cal_det(mat_copy, co);

    vector<double> d_vec(mat[0].size() - 1, 0);

    for (int i = 0; i < d_vec.size(); ++i) {
        mat_copy = mat;
        swap_cols(mat_copy, i, mat[0].size() - 1);
        co = convert_matrix_to_stepped(mat_copy);
        print_matrix(mat_copy);
        d_vec[i] = cal_det(mat_copy, co);
    }

    cout << "linear equation solve is: " << endl;
    for (int i = 0; i < d_vec.size(); ++i) {
        cout << d_vec[i] / d << ' ';
    }
    cout << endl;

    return 0;
}
