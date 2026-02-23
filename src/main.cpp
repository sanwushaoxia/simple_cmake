#include "../include/main.h"

// int main()
// {
//     // cal_vec_sum_with_multiprocessing();
//     // virtualFunc();
//     // constructor_class();
//     // producer_consumer_model();
//     write_shared_memory();
//     read_shared_memory();
//     return 0;
// }

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int d[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

bool dfs(vector<vector<int>> &mat, int x, int y, vector<vector<int>> &path, int total, int cnt)
{
    mat[x][y] = 1;
    cout << x << ' ' << y << ' ' << path.size() << endl;
    path.emplace_back(vector<int>{x, y});
    bool ret;
    if (cnt == total) {
        return true;
    }
    for (int k = 0; k < 4; ++k) {
        int i = x + d[k][0];
        int j = y + d[k][1];
        if (i < 0 || i >= mat.size() || j < 0 || j >= mat[0].size() || mat[i][j] == 1) {
            continue;
        }
        ret = dfs(mat, i, j, path, total, cnt + 1);
        if (ret == true) {
            return true;
        }
    }
    mat[x][y] = 0;
    path.pop_back();
    return false;
}

// 0 表示空地
int solve(vector<vector<int>> &mat, int x, int y)
{
    int cnt = 0; // 空地数
    for (int i = 0; i < mat.size(); ++i) {
        for (int j = 0; j < mat[0].size(); ++j) {
            if (mat[i][j] == 0) {
                ++cnt;
            }
        }
    }
    cout << cnt << endl;

    vector<vector<int>> path(0, vector<int>(2, 0));
    dfs(mat, x, y, path, cnt, 1);

    for (const auto &v : path) {
        cout << "x=" << v[0] << ' ' << "y=" << v[1] << endl;
    }
    return 0;
}

int main()
{
    // vector<vector<int>> mat{{1, 0, 1, 0, 1}, {0, 0, 1, 0, 0}, {0, 1, 0, 1, 0}, {0, 0, 0, 0, 0}, {1, 1, 0, 0, 1}};
    // solve(mat, 4, 2);

    // socket_model();
    // list_sort();
    // weakPtr();
    // constructor_class();
    // write_shared_memory();
    // read_shared_memory();
    // generator();
    // cal_inverse_order();
    solve_linear_equations();

    return 0;
}


