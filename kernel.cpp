#include <stdint.h>
#include <stddef.h>

/* we use extern "C" here many times to avoid C++ linkage which does name mangling and to use C style linkage because i need to
and to why not just c but don't feel like dealing with macros , name collisons and resource cleanup while i am already
having hard time dealing with buidling an OS itself 
*/ 

//linker script symbols 
extern "C" {
    extern char __bss[];
    extern char __bss_end[];
    extern char __stack_top[];
}

extern "C" void* memset(void *buf, char c, size_t n) {
    uint8_t *p = static_cast<uint8_t*>(buf);
    while (n--) {
        *p++ = static_cast<uint8_t>(c);
    }
    return buf;
}

extern "C" void kernel_main(void) {
    // Calculate BSS size and clear it to zero
    size_t bss_size = reinterpret_cast<uintptr_t>(__bss_end) - reinterpret_cast<uintptr_t>(__bss);
    memset(__bss, 0, bss_size);

    // Main kernel loop
    for (;;) {
        asm volatile("wfi"); // Wait for interrupt (saves CPU usage in QEMU)
    }
}

// Boot entry point referenced by ENTRY(boot) in kernel.ld
extern "C" __attribute__((section(".text.boot"), naked)) void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Load initial stack pointer
        "j kernel_main\n"       // Jump to kernel_main
        :
        : [stack_top] "r" (__stack_top)
    );
}