#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xcd71858e, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x62fc2946, __VMLINUX_SYMBOL_STR(param_ops_uint) },
	{ 0x4b88c2fb, __VMLINUX_SYMBOL_STR(param_ops_long) },
	{ 0x4068df6a, __VMLINUX_SYMBOL_STR(ethtool_op_get_link) },
	{ 0x3e7edda2, __VMLINUX_SYMBOL_STR(usbnet_tx_timeout) },
	{ 0xa5dbfb11, __VMLINUX_SYMBOL_STR(usbnet_change_mtu) },
	{ 0x6e3e46e, __VMLINUX_SYMBOL_STR(eth_validate_addr) },
	{ 0x21dbed8e, __VMLINUX_SYMBOL_STR(usbnet_stop) },
	{ 0xf5948e3e, __VMLINUX_SYMBOL_STR(usb_deregister) },
	{ 0x7e6ed3ca, __VMLINUX_SYMBOL_STR(usb_register_driver) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x3c3fce39, __VMLINUX_SYMBOL_STR(__local_bh_enable_ip) },
	{ 0x293244b, __VMLINUX_SYMBOL_STR(netif_schedule_queue) },
	{ 0x6bf1c17f, __VMLINUX_SYMBOL_STR(pv_lock_ops) },
	{ 0x7a2af7b4, __VMLINUX_SYMBOL_STR(cpu_number) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x4629334c, __VMLINUX_SYMBOL_STR(__preempt_count) },
	{ 0x4acc3f85, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0xd45ea32f, __VMLINUX_SYMBOL_STR(hrtimer_start_range_ns) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xe6da44a, __VMLINUX_SYMBOL_STR(set_normalized_timespec) },
	{ 0x6de17c55, __VMLINUX_SYMBOL_STR(netif_rx_ni) },
	{ 0x91ad9692, __VMLINUX_SYMBOL_STR(arp_create) },
	{ 0x349cba85, __VMLINUX_SYMBOL_STR(strchr) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0x53569707, __VMLINUX_SYMBOL_STR(this_cpu_off) },
	{ 0xe8b8978e, __VMLINUX_SYMBOL_STR(netif_receive_skb_sk) },
	{ 0x40ac9bda, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0xe9cef686, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0x79aa04a2, __VMLINUX_SYMBOL_STR(get_random_bytes) },
	{ 0x6aea0610, __VMLINUX_SYMBOL_STR(usb_string) },
	{ 0xab7d6a08, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xf1669456, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xf5477c46, __VMLINUX_SYMBOL_STR(usb_cdc_wdm_register) },
	{ 0x87af3bee, __VMLINUX_SYMBOL_STR(usbnet_get_endpoints) },
	{ 0x9545af6d, __VMLINUX_SYMBOL_STR(tasklet_init) },
	{ 0x9885e6ad, __VMLINUX_SYMBOL_STR(usbnet_probe) },
	{ 0xd7a71b2, __VMLINUX_SYMBOL_STR(dev_queue_xmit_sk) },
	{ 0x54312d2f, __VMLINUX_SYMBOL_STR(usbnet_skb_return) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xcda5cedb, __VMLINUX_SYMBOL_STR(skb_pull) },
	{ 0x539c6cb1, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0xb692a514, __VMLINUX_SYMBOL_STR(__netdev_alloc_skb) },
	{ 0xbeb2eef2, __VMLINUX_SYMBOL_STR(ether_setup) },
	{ 0x2e0d2f7f, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xef02741c, __VMLINUX_SYMBOL_STR(netdev_rx_handler_register) },
	{ 0xafac7eb4, __VMLINUX_SYMBOL_STR(netdev_info) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x8cc90047, __VMLINUX_SYMBOL_STR(netdev_err) },
	{ 0xbec8fc83, __VMLINUX_SYMBOL_STR(netif_device_attach) },
	{ 0xd2eac02, __VMLINUX_SYMBOL_STR(register_netdev) },
	{ 0x949f7342, __VMLINUX_SYMBOL_STR(__alloc_percpu) },
	{ 0xd56b5f64, __VMLINUX_SYMBOL_STR(ktime_get_ts64) },
	{ 0x83ba5fbb, __VMLINUX_SYMBOL_STR(hrtimer_init) },
	{ 0x8667032a, __VMLINUX_SYMBOL_STR(alloc_netdev_mqs) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x60510b9, __VMLINUX_SYMBOL_STR(usbnet_disconnect) },
	{ 0x82072614, __VMLINUX_SYMBOL_STR(tasklet_kill) },
	{ 0x47c9c5de, __VMLINUX_SYMBOL_STR(free_netdev) },
	{ 0x973c6ccb, __VMLINUX_SYMBOL_STR(unregister_netdev) },
	{ 0xc9ec4e21, __VMLINUX_SYMBOL_STR(free_percpu) },
	{ 0x6e720ff2, __VMLINUX_SYMBOL_STR(rtnl_unlock) },
	{ 0xcbe61235, __VMLINUX_SYMBOL_STR(netdev_rx_handler_unregister) },
	{ 0xc7a4fbed, __VMLINUX_SYMBOL_STR(rtnl_lock) },
	{ 0x1916e38c, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x93a9e57d, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0x680ec266, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0x88bfa7e, __VMLINUX_SYMBOL_STR(cancel_work_sync) },
	{ 0xe418fde4, __VMLINUX_SYMBOL_STR(hrtimer_cancel) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x5c67605a, __VMLINUX_SYMBOL_STR(usbnet_resume) },
	{ 0x36f9630f, __VMLINUX_SYMBOL_STR(usbnet_suspend) },
	{ 0x58620388, __VMLINUX_SYMBOL_STR(usbnet_open) },
	{ 0xca335d5b, __VMLINUX_SYMBOL_STR(usbnet_start_xmit) },
	{ 0xc3636cb7, __VMLINUX_SYMBOL_STR(eth_commit_mac_addr_change) },
	{ 0xa2294477, __VMLINUX_SYMBOL_STR(eth_prepare_mac_addr_change) },
	{ 0x5792f848, __VMLINUX_SYMBOL_STR(strlcpy) },
	{ 0xb858cefe, __VMLINUX_SYMBOL_STR(usbnet_get_drvinfo) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0x1cb7f072, __VMLINUX_SYMBOL_STR(netif_tx_wake_queue) },
	{ 0xbd213fc4, __VMLINUX_SYMBOL_STR(netif_carrier_on) },
	{ 0x909ba38b, __VMLINUX_SYMBOL_STR(netif_carrier_off) },
	{ 0xfaef0ed, __VMLINUX_SYMBOL_STR(__tasklet_schedule) },
	{ 0xfe7c4287, __VMLINUX_SYMBOL_STR(nr_cpu_ids) },
	{ 0xc0a3d105, __VMLINUX_SYMBOL_STR(find_next_bit) },
	{ 0x3928efe9, __VMLINUX_SYMBOL_STR(__per_cpu_offset) },
	{ 0xb9249d16, __VMLINUX_SYMBOL_STR(cpu_possible_mask) },
	{ 0xb2be6e92, __VMLINUX_SYMBOL_STR(netdev_stats_to_stats64) },
	{ 0x20000329, __VMLINUX_SYMBOL_STR(simple_strtoul) },
	{ 0xb3de4b85, __VMLINUX_SYMBOL_STR(usb_control_msg) },
	{ 0xbfc883f6, __VMLINUX_SYMBOL_STR(usb_driver_release_interface) },
	{ 0x5520efd0, __VMLINUX_SYMBOL_STR(__dynamic_dev_dbg) },
	{ 0x504864b1, __VMLINUX_SYMBOL_STR(usb_autopm_put_interface) },
	{ 0x72f6a3e4, __VMLINUX_SYMBOL_STR(usb_autopm_get_interface) },
	{ 0x31ba5de0, __VMLINUX_SYMBOL_STR(__dev_kfree_skb_any) },
	{ 0xbf9606ad, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x963089c5, __VMLINUX_SYMBOL_STR(_dev_info) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=usbnet,cdc-wdm";

MODULE_ALIAS("usb:v05C6pF601d*dc*dsc*dp*ic*isc*ip*in05*");
MODULE_ALIAS("usb:v2DEEp4D22d*dc*dsc*dp*ic*isc*ip*in05*");

MODULE_INFO(srcversion, "55EC74F08177810213604C6");
