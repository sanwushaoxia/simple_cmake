#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

const char* shm_name = "/my_shm";
const size_t shm_size = 4096;

int write_shared_memory() {
    // 创建或打开共享内存对象
    int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    // 设置共享内存大小
    if (ftruncate(fd, shm_size) == -1) {
        perror("ftruncate");
        return 1;
    }

    // 映射到进程地址空间
    void* ptr = mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // 写入数据
    const char* msg = "Hello from writer!";
    std::strcpy(static_cast<char*>(ptr), msg);
    std::cout << "Writer: wrote message to shared memory.\n";

    // 清理（注意：不要 unlink，否则 reader 无法访问）
    munmap(ptr, shm_size);
    close(fd);

    // 可选：程序退出前删除共享内存对象（通常由最后一个使用者清理）
    // shm_unlink(shm_name);
    return 0;
}

int read_shared_memory() {
    int fd = shm_open(shm_name, O_RDONLY, 0666);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

    void* ptr = mmap(nullptr, shm_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    std::cout << "Reader: read message: " << static_cast<char*>(ptr) << '\n';

    munmap(ptr, shm_size);
    close(fd);

    // 删除共享内存对象（由 reader 或 writer 负责清理一次即可）
    shm_unlink(shm_name);
    return 0;
}
