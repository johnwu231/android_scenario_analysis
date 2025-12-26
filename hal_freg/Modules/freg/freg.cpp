#define LOG_TAG "FreqHALStub"
#include <hardware/hardware.h>
#include <hardware/freq.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#define DEVICE_NAME "/dev/freq"
#define MODULE_NAME "Freq"
#define MODULE_AUTHOR "shyluo@gmail.com"
/*设备打开和关闭接口*/
static int freq_device_open(const struct hw_module_t *module, const char *id, struct hw_device_t **device);
static int freq_device_close(struct hw_device_t *device);
/*设备存储器读写接口*/
static int freq_get_val(struct freq_device_t *dev, int *val);
static int freq_set_val(struct freq_device_t *dev, int val);
/*定义模块操作方法结构体变量*/
static struct hw_module_methods_t freq_module_methods = {
    open : freq_device_open
};
/*定义模块结构体变量*/
struct freq_module_t HAL_MODULE_INFO_SYM = {
    common : {
        tag : HARDWARE_MODULE_TAG,
        version_major : 1,
        version_minor : 0,
        id : FREG_HARDWARE_MODULE_ID,
        name : MODULE_NAME,
        author : MODULE_AUTHOR,
        methods : &freq_module_methods,
    }
};

static int freq_device_open(const struct hw_module_t *module, const char *id, struct hw_device_t **device)
{
    if (!strcmp(id, FREG_HARDWARE_DEVICE_ID))
    {
        struct freq_device_t *dev;

        dev = (struct freq_device_t *)malloc(sizeof(struct freq_device_t));
        if (!dev)
        {
            LOGE("Failed to alloc space for freq_device_t.");
            return -EFAULT;
        }

        memset(dev, 0, sizeof(struct freq_device_t));

        dev->common.tag = HARDWARE_DEVICE_TAG;
        dev->common.version = 0;
        dev->common.module = (hw_module_t *)module;
        dev->common.close = freq_device_close;
        dev->set_val = freq_set_val;
        dev->get_val = freq_get_val;

        if ((dev->fd = open(DEVICE_NAME, O_RDWR)) == -1)
        {
            LOGE("Failed to open device file /dev/freq -- %s.", strerror(errno));
            free(dev);
            return -EFAULT;
        }

        *device = &(dev->common);

        LOGI("Open device file /dev/freq successfully.");
        return 0;
    }

    return -EFAULT;
}

static int freq_device_close(struct hw_device_t *device)
{
    struct freq_device_t *freq_device = (struct freq_device_t *)device;
    if (freq_device)
    {
        close(freq_device->fd);
        free(freq_device);
    }
    return 0;
}

static int freq_get_val(struct freq_device_t *dev, int *val)
{
    if (!dev)
    {
        LOGE("Null dev pointer.");
        return -EFAULT;
    }
    if (!val)
    {
        LOGE("Null val pointer.");
        return -EFAULT;
    }
    read(dev->fd, val, sizeof(*val));
    LOGI("Get value %d from device file /dev/freq.", *val);
    return 0;
}

static int freq_set_val(struct freq_device_t *dev, int val)
{
    if (!dev)
    {
        LOGE("Null dev pointer.");
        return -EFAULT;
    }
    LOGI("Set value %d to device file /dev/freq.", val);
    write(dev->fd, &val, sizeof(val));
    return 0;
}