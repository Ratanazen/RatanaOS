#include "../../include/vfs.h"
#include "../../include/string.h"
#include "../../include/serial.h"

#define DECL_ELF(name) \
    extern unsigned char build_##name##_elf[]; \
    extern unsigned int build_##name##_elf_len; \
    static vfs_node_t name##_node;

DECL_ELF(init)
DECL_ELF(hello)
DECL_ELF(sh)
DECL_ELF(ls)
DECL_ELF(cat)
DECL_ELF(echo)
DECL_ELF(pwd)
DECL_ELF(uname)
DECL_ELF(touch)
DECL_ELF(mkdir)
DECL_ELF(rm)
DECL_ELF(free)
DECL_ELF(ps)
DECL_ELF(date)
DECL_ELF(clear)
DECL_ELF(ratapkg)
DECL_ELF(debimport)
DECL_ELF(ip)
DECL_ELF(ping)
DECL_ELF(grep)
DECL_ELF(head)
DECL_ELF(tail)
DECL_ELF(wc)
DECL_ELF(sort)
DECL_ELF(sleep)
DECL_ELF(kill)
DECL_ELF(env)

static void register_app(vfs_node_t* node, const char* name, unsigned char* data, unsigned int len) {
    memset(node, 0, sizeof(vfs_node_t));
    strcpy(node->name, name);
    node->flags = VFS_FILE;
    node->size = len;
    node->data = data;
    node->fs_type = FS_RAMFS;
    node->next = NULL;
    vfs_register_node(node);
    serial_printf("INITRAMFS: Registered %s (%d bytes)\n", name, len);
}

void initramfs_init(void) {
    register_app(&init_node, "/sbin/init", build_init_elf, build_init_elf_len);
    register_app(&hello_node, "/bin/hello", build_hello_elf, build_hello_elf_len);
    register_app(&sh_node, "/bin/sh", build_sh_elf, build_sh_elf_len);
    register_app(&ls_node, "/bin/ls", build_ls_elf, build_ls_elf_len);
    register_app(&cat_node, "/bin/cat", build_cat_elf, build_cat_elf_len);
    register_app(&echo_node, "/bin/echo", build_echo_elf, build_echo_elf_len);
    register_app(&pwd_node, "/bin/pwd", build_pwd_elf, build_pwd_elf_len);
    register_app(&uname_node, "/bin/uname", build_uname_elf, build_uname_elf_len);
    register_app(&touch_node, "/bin/touch", build_touch_elf, build_touch_elf_len);
    register_app(&mkdir_node, "/bin/mkdir", build_mkdir_elf, build_mkdir_elf_len);
    register_app(&rm_node, "/bin/rm", build_rm_elf, build_rm_elf_len);
    register_app(&free_node, "/bin/free", build_free_elf, build_free_elf_len);
    register_app(&ps_node, "/bin/ps", build_ps_elf, build_ps_elf_len);
    register_app(&date_node, "/bin/date", build_date_elf, build_date_elf_len);
    register_app(&clear_node, "/bin/clear", build_clear_elf, build_clear_elf_len);
    register_app(&ratapkg_node, "/bin/ratapkg", build_ratapkg_elf, build_ratapkg_elf_len);
    register_app(&debimport_node, "/bin/debimport", build_debimport_elf, build_debimport_elf_len);
    register_app(&ip_node, "/bin/ip", build_ip_elf, build_ip_elf_len);
    register_app(&ping_node, "/bin/ping", build_ping_elf, build_ping_elf_len);
    register_app(&grep_node, "/bin/grep", build_grep_elf, build_grep_elf_len);
    register_app(&head_node, "/bin/head", build_head_elf, build_head_elf_len);
    register_app(&tail_node, "/bin/tail", build_tail_elf, build_tail_elf_len);
    register_app(&wc_node, "/bin/wc", build_wc_elf, build_wc_elf_len);
    register_app(&sort_node, "/bin/sort", build_sort_elf, build_sort_elf_len);
    register_app(&sleep_node, "/bin/sleep", build_sleep_elf, build_sleep_elf_len);
    register_app(&kill_node, "/bin/kill", build_kill_elf, build_kill_elf_len);
    register_app(&env_node, "/bin/env", build_env_elf, build_env_elf_len);
}
