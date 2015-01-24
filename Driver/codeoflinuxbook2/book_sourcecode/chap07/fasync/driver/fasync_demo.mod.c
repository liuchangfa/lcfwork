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
	{ 0x9412fa01, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xd743757b, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x268f87ce, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xc9296d71, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x58df1fc2, __VMLINUX_SYMBOL_STR(device_remove_file) },
	{ 0x39a1e906, __VMLINUX_SYMBOL_STR(device_create_file) },
	{ 0xda6982e5, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xd7bd463f, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x987fde2e, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xb46aca86, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x489ac673, __VMLINUX_SYMBOL_STR(cdev_alloc) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x2b8aacb7, __VMLINUX_SYMBOL_STR(fasync_helper) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x882d57fd, __VMLINUX_SYMBOL_STR(kill_fasync) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "E6442C53B74E5E6D2BD40A8");
