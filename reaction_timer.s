////////////////////////////////////////////////////////////////////////////
// Author: Pedro Carneiro Passos
//
// Purpose: This is used to demonstrate and debug code that was compiled
//          at Compiler Explorer.
///////////////////////////////////////////////////////////////////////////
//
// This is the interrupt vector table. It must begin at address 0x00000000
// so do not put any executable code above this.
// Note that only the RESET and IRQ vectors actually do anything. The rest
// are just infinite loops.
//
////////////////////////////////////////////////////////////////////////////
    B SERVICE_RESET    // reset vector
SERVICE_UND:
    B SERVICE_UND      // undefined instruction
SERVICE_SVC:
    B SERVICE_SVC      // software interrrupt
SERVICE_ABT_INST:
    B SERVICE_ABT_INST // aborted instruction prefetch
SERVICE_ABT_DATA:
    B SERVICE_ABT_DATA // aborted data fetch
    .word 0            // unused
    B SERVICE_IRQ      // IRQ interrupt
SERVICE_FIQ:
    B SERVICE_FIQ      // FIQ interrupt
////////////////////////////////////////////////////////////////////////////
// Create the global and static variables
////////////////////////////////////////////////////////////////////////////

//Create the global variable that counts interrupts.
.global count
	count: .word 0x0
////////////////////////////////////////////////////////////////////////////
config_gic:
        bic     r1, r0, #3
        lsr     r3, r0, #5
        sub     r1, r1, #77824
        add     r3, r3, #64
        push    {r4, lr}
        and     r4, r0, #3
        ldr     lr, [r1, #2048]
        mov     r2, #1
        movw    ip, #53503
        lsl     r4, r4, #3
        movt    ip, 65534
        orr     lr, lr, r2, lsl r4
        add     r3, ip, r3, lsl #2
        str     lr, [r1, #2048]
        movw    r1, #49663
        movt    r1, 65534
        and     r0, r0, #31
        lsl     r0, r2, r0
        str     r0, [r3, #-255]
        str     r2, [r1, #-255]
        str     r2, [ip, #-255]
        pop     {r4, pc}
SERVICE_IRQ:
        push    {r0, r1, r2, r3, ip}
        movw    r3, #49663
        movt    r3, 65534
        ldr     r2, [r3, #-243]
        cmp     r2, #72
        bne     .L5
        movw    r3, #:lower16:count
        movt    r3, #:upper16:count
        mov     r0, #8192
        movt    r0, 65312
        ldr     r1, [r3]
        mov     ip, #1
        add     r1, r1, ip
        str     r1, [r3]
        str     ip, [r0]
.L5:
        movw    r3, #49663
        movt    r3, 65534
        str     r2, [r3, #-239]
        pop     {r0, r1, r2, r3, ip}
        subs    pc, lr, #4
setup:
        movw    r2, #:lower16:count
        mov     r3, #8192
        movt    r2, #:upper16:count
        movt    r3, 65312
        mov     r1, #0
        movw    r0, #16960
        str     r1, [r2]
        mov     r1, #15
        str     r0, [r3, #8]
        mov     r2, #7
        str     r1, [r3, #12]
        str     r2, [r3, #4]
        bx      lr
print_char:
        mov     r2, #4096
        movt    r2, 65312
        ldr     r3, [r2, #4]
        and     r3, r3, #16711680
        cmp     r3, #4
        strhi   r0, [r2]
        bx      lr
print_message.constprop.0:
        push    {r4, lr}
        mov     r0, #70
        bl      print_char
        mov     r0, #65
        bl      print_char
        mov     r0, #73
        bl      print_char
        mov     r0, #76
        bl      print_char
        mov     r0, #10
        pop     {r4, lr}
        b       print_char
wait_release:
        mov     r2, #0
        movt    r2, 65312
.L13:
        ldr     r3, [r2, #80]
        tst     r3, #15
        bne     .L13
        bx      lr
display_leds_nine_zero:
        mov     r3, #0
        movt    r3, 65312
        movw    r2, #513
        str     r2, [r3]
        bx      lr
wait_one_sec:
        movw    r2, #:lower16:count
        movt    r2, #:upper16:count
        ldr     r1, [r2]
.L18:
        ldr     r3, [r2]
        sub     r3, r3, r1
        cmp     r3, #99
        bls     .L18
        bx      lr
turn_off:
        mov     r3, #0
        movt    r3, 65312
        mov     r2, #0
        str     r2, [r3]
        bx      lr
random_led:
        tst     r0, #512
        mov     r3, #0
        movt    r3, 65312
        movne   r2, #512
        moveq   r2, #1
        str     r2, [r3]
        bx      lr
print_message:
        subs    r1, r0, #0
        push    {r4, lr}
        beq     .L29
        movw    r2, #34079
        movt    r2, 20971
        mov     ip, #100
        umull   r2, r3, r1, r2
        lsr     r4, r3, #5
        add     r0, r4, #48
        mls     r4, ip, r4, r1
        bl      print_char
        mov     r0, #46
        bl      print_char
        movw    r2, #52429
        movt    r2, 52428
        umull   r2, r3, r4, r2
        lsr     r1, r3, #3
        add     r0, r1, #48
        bl      print_char
        mov     r0, #10
        mls     r0, r0, r1, r4
        add     r0, r0, #48
        bl      print_char
        mov     r0, #10
        pop     {r4, lr}
        b       print_char
.L29:
        mov     r0, #70
        bl      print_char
        mov     r0, #65
        bl      print_char
        mov     r0, #73
        bl      print_char
        mov     r0, #76
        bl      print_char
        mov     r0, #10
        pop     {r4, lr}
        b       print_char
response:
        movw    r1, #:lower16:count
        movt    r1, #:upper16:count
        mov     r2, #0
        movt    r2, 65312
        ldr     r0, [r1]
.L37:
        ldr     r3, [r1]
        sub     r3, r3, r0
        cmp     r3, #199
        bhi     .L42
.L33:
        ldr     r3, [r2, #80]
        tst     r3, #8
        beq     .L32
        ldr     r3, [r2]
        cmp     r3, #512
        beq     .L43
.L32:
        ldr     r3, [r2, #80]
        tst     r3, #1
        beq     .L37
        ldr     r3, [r2]
        cmp     r3, #1
        bne     .L37
        ldr     r3, [r2, #80]
        tst     r3, #14
        bne     .L37
.L40:
        ldr     r3, [r1]
        sub     r0, r3, r0
        b       print_message
.L43:
        ldr     r3, [r2, #80]
        tst     r3, #7
        beq     .L40
        ldr     r3, [r1]
        sub     r3, r3, r0
        cmp     r3, #199
        bls     .L33
.L42:
        b       print_message.constprop.0
loop:
        mov     r3, #8192
        movt    r3, 65312
        push    {r4, lr}
        mov     r2, #41
        str     r2, [r3, #16]
        ldr     r0, [r3, #16]
        bl      wait_release
        bl      display_leds_nine_zero
        bl      wait_one_sec
        bl      turn_off
        bl      wait_one_sec
        bl      random_led
        pop     {r4, lr}
        b       response
SERVICE_RESET:
        push    {r4, lr}
        mov     r3, #2048
        MOV  sp, r3
        mov     r3, #18
        MSR  cpsr_c, r3
        mov     r3, #1792
        MOV  sp, r3
        mov     r3, #19
        MSR  cpsr_c, r3
        mov     r0, #72
        bl      config_gic
        bl      setup
        cpsie i
.L47:
        bl      loop
        b       .L47
