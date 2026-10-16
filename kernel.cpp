#include <stdint.h>
#include <stddef.h>

/* we use extern "C" here many times to avoid C++ linkage which does name mangling and to use C style linkage because i need to
and to why not just c but don't feel like dealing with macros , name collisons and resource cleanup while i am already
having hard time dealing with buidling an OS itself
*/

// linker script symbols
extern "C"
{
    extern char __bss[];
    extern char __bss_end[];
    extern char __stack_top[];
}

// Fills 'n' bytes of memory starting at 'buf' with character 'c'
extern "C" void *memset(void *buf, char c, size_t n)
{
    uint8_t *p = static_cast<uint8_t *>(buf); // Cast to 8-bit pointer for byte-by-byte movement
    while (n--)
    {
        *p++ = static_cast<uint8_t>(c); // Write byte value and advance pointer by 1 byte
    }
    return buf;
}

extern "C" void kernel_main(void)
{
    // Subtract start address from end address to get total BSS byte count
    size_t bss_size = reinterpret_cast<uintptr_t>(__bss_end) - reinterpret_cast<uintptr_t>(__bss);

    // Wipe .bss in RAM to 0 so uninitialized globals start clean
    memset(__bss, 0, bss_size);

    // Halt CPU until next interrupt (prevents 100% CPU usage in QEMU)
    for (;;) // infinite loop coz the kernel needs to keep running and not return anything at the end
    {
        asm volatile("wfi"); // volatile so cpp compiler doesn't think of it as useless code
    }
}

// Early boot assembly entry point (placed at 0x80200000 by kernel.ld)[cite: 6]
// forces to put this fuction to section .text.boot to get it to exactly the address where the opensbi jumps to
// "naked" forces the compiler to not run it like normal cpp function and not put in stack at start because the stack doesn't exists yet
// both together basically make the asembly written inside run directly in cpu registers and program counter which then marks the stack top to sp register which is stack pointer and the shit from now can execute in stack for other os stuff
extern "C" __attribute__((section(".text.boot"), naked)) void boot(void)
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set RISC-V stack pointer register to top of reserved RAM stack
        "j kernel_main\n"       // Jump to C++ kernel entry point
        :
        : [stack_top] "r"(__stack_top));
}