#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

using namespace std;

int cal_vec_sum_with_multiprocessing()
{
    vector<int> vec;
    for (int i = 1; i != 100; ++i) {
        vec.emplace_back(i);
    }
    int num_processes = 8;
    int chunk_size = (vec.size() + num_processes - 1) / num_processes;

    vector<vector<int>> pipes(num_processes, vector<int>(2, 0));
    for (auto &v : pipes) {
        if (pipe(v.data()) < 0) {
            perror("pipe");
            return 1;
        }
    }

    vector<int> pids(num_processes, 0);
    for (int i = 0; i != num_processes; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            for (int j = 0; j != num_processes; ++j) {
                close(pipes[j][0]);
                if (j != i) {
                    close(pipes[j][1]);
                }
            }

            int local_sum = 0;
            int beg = i * chunk_size;
            int end = MIN(beg + chunk_size, vec.size());
            for (int j = beg; j != end; ++j) {
                local_sum += vec[j];
            }
            write(pipes[i][1], &local_sum, sizeof(local_sum));
            close(pipes[i][1]);
            return 0;
        } else {
            pids[i] = pid;
            close(pipes[i][1]);
        }
    }

    int total_sum = 0;
    for (int i = 0; i != num_processes; ++i) {
        int local_sum;
        read(pipes[i][0], &local_sum, sizeof(local_sum));
        close(pipes[i][0]);
        cout << "local_sum = " << local_sum << endl;
        total_sum += local_sum;
    }
    cout << "total_sum = " << total_sum << endl;
    for (int i = 0; i != num_processes; ++i) {
        waitpid(pids[i], nullptr, 0);
    }

    return 0;
}
