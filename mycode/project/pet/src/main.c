#include <zephyr/kernel.h>
#include <zephyr/fs/fs.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

#define MAX_PATH_LEN 255
#define MOUNT_POINT "/lfs1"

void main(void)
{
    struct fs_file_t file;
    char path[MAX_PATH_LEN];
    char read_buf[64];
    ssize_t bytes_read;
    int rc;

    snprintf(path, sizeof(path), "%s/hello.txt", MOUNT_POINT);
    fs_file_t_init(&file);

    rc = fs_open(&file, path, FS_O_CREATE | FS_O_WRITE);
    if (rc < 0) {
        LOG_ERR("Failed to create file: %d", rc);
        return;
    }

    rc = fs_write(&file, "Hello World!", strlen("Hello World!"));
    if (rc < 0) {
        LOG_ERR("Failed to write to file: %d", rc);
        fs_close(&file);
        return;
    }

    LOG_INF("File created and written successfully");
    fs_close(&file);

    rc = fs_open(&file, path, FS_O_READ);
    if (rc < 0) {
        LOG_ERR("Failed to open file for reading: %d", rc);
        return;
    }

    bytes_read = fs_read(&file, read_buf, sizeof(read_buf));
    if (bytes_read < 0) {
        LOG_ERR("Failed to read file: %d", bytes_read);
    } else {
        read_buf[bytes_read] = '\0';
        LOG_INF("Read from file: %s", read_buf);
    }

    fs_close(&file);
}