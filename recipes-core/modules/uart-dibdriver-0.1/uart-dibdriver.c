#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/serdev.h>
#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/random.h>

static int serdev_echo_recv(struct serdev_device *serdev, const u8 *data, size_t count)
{
    size_t i;

    pr_info("Received %zu bytes: ", count);
    for (i = 0; i < count; i++)
        pr_cont("%02x", data[i]);
    pr_cont("\n");

    u8 random_byte;
    get_random_bytes(&random_byte, sizeof(random_byte));
    return serdev_device_write_buf(serdev, &random_byte, sizeof(random_byte));
}

static const struct serdev_device_ops serdev_echo_ops = {
    .receive_buf = serdev_echo_recv,
};

static int custom_uart_probe(struct serdev_device *serdev)
{
    serdev_device_set_client_ops(serdev, &serdev_echo_ops);
    int status = serdev_device_open(serdev);
	if(status) {
		printk("Error opening serial port!\n");
		return -status;
	}
	serdev_device_set_baudrate(serdev, 115200);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);
	serdev_device_set_flow_control(serdev, false);

    status = serdev_device_write_buf(serdev, "aa", sizeof("aa"));

    return 0;
}

static void custom_uart_remove(struct serdev_device *serdev)
{
	serdev_device_close(serdev);
}

static const struct of_device_id custom_uart_of_match[] = {
    { .compatible = "fakedevice,mocne", },
    { /* sentinel */ }
};

static struct serdev_device_driver custom_uart_driver = {
    .driver = {
        .name = "custom_uart",
        .of_match_table = custom_uart_of_match,
    },
    .probe = custom_uart_probe,
    .remove = custom_uart_remove,
};


MODULE_AUTHOR("lampaBiurkowa");
MODULE_DESCRIPTION("Sample UART Driver");
MODULE_LICENSE("GPL");

static int __init my_init(void) {
	printk("Loading the driver...\n");
    int status = serdev_device_driver_register(&custom_uart_driver);
    if (status)
    {
        printk("Error loading the driver\n");
    }
	printk("Loaded the driver\n");

	return 0;
}

// /**
//  * @brief This function is called, when the module is removed from the kernel
//  */
static void __exit my_exit(void) {
	printk("Unload driver");
	serdev_device_driver_unregister(&custom_uart_driver);
}

module_init(my_init);
module_exit(my_exit);