#define EXEC_2_TIMES(x)   x x
#define EXEC_4_TIMES(x)   EXEC_2_TIMES(EXEC_2_TIMES(x))
#define EXEC_8_TIMES(x)   EXEC_2_TIMES(EXEC_4_TIMES(x))
#define EXEC_16_TIMES(x)  EXEC_2_TIMES(EXEC_8_TIMES(x))
#define EXEC_32_TIMES(x)   EXEC_2_TIMES(EXEC_16_TIMES(x))
#define EXEC_64_TIMES(x)   EXEC_2_TIMES(EXEC_32_TIMES(x))
#define EXEC_128_TIMES(x)   EXEC_2_TIMES(EXEC_64_TIMES(x))
#define EXEC_256_TIMES(x)   EXEC_2_TIMES(EXEC_128_TIMES(x))

//used 
#define UNROLL_CONSTANT 1024
#ifdef UNROLL_JUMPS
	#define UNROLL_OPT(x) EXEC_64_TIMES(x)
	#define ACCESSES_PER_LOOP 64
#endif
#ifndef UNROLL_JUMPS
	#define UNROLL_OPT(x) x
	#define ACCESSES_PER_LOOP 1
#endif

// rcx: bitmask used in LFSR NOT for the result 

// r8: access offset 
//sets r12 to one or zero (not only r12d)
//sets r12 either to 0 (if zero) or is filled with ones
//apply mask only if last bit is one
//"and    %[accessmask], %%r8 \n" // not needed atm, because 
#define GET_NEXT_RANDOM_VALUE \
        asm volatile( \
            "movabs $0xd800000000000000, %%rcx \n" 	\
            "xor %%r8, %%r8 \n"					   \
            "xor %%r11, %%r11 \n"                  \
            "mov    %[random], %%r9 \n"            \
            "mov    %%r9, %%r12 \n"                \
            "shr    %%r9 \n"                       \
            "and    $0x1, %%r12d \n"               \
            "neg    %%r12 \n"                      \
            "and    %%rcx, %%r12 \n"               \
            "xor    %%r9, %%r12 \n"                \
            "mov    %%r12, %[random] \n"           \
            "mov    %%r12, %%r8 \n"                \
            \
            "mov    %%r8, %[rand_val] \n"          \
            :[random] "=r" (rand_seed), [rand_val] "=r" (rand_val) \
            : "0"(rand_seed), [accessmask] "r" (access_mask) \
            : "%rcx", "%r12", "%r11", "%r10", "%r9", "%r8" \
        );

#define READ_ADDR_ADD_8192 \
	"add $8192, %[addr]\n"

#define READ_NEXT_ADDR \
	"movq %%xmm0, %[next_addr]\n"

#define READ_OFFSET

#define READ_NT_64_ASM \
    "vmovntdqa 0(%[addr]), %%zmm0 \n"

#define READ_NT_128_ASM \
    "vmovntdqa 0(%[addr]), %%zmm0 \n"\
    "vmovntdqa 1*64(%[addr]), %%zmm1 \n"

#define READ_NT_256_ASM \
    "vmovntdqa 0(%[addr]),      %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm1 \n"

#define READ_NT_512_ASM \
    "vmovntdqa 0*64(%[addr]),   %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 4*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 5*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 6*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 7*64(%[addr]),   %%zmm1 \n" 

#define READ_NT_1024_ASM \
    "vmovntdqa 0*64(%[addr]),   %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 4*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 5*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 6*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 7*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 8*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 9*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 10*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 11*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 12*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 13*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 14*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 15*64(%[addr]),   %%zmm1 \n" \

#define READ_NT_2048_ASM \
    "vmovntdqa 0*64(%[addr]),   %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 4*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 5*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 6*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 7*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 8*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 9*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 10*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 11*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 12*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 13*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 14*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 15*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 16*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 17*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 18*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 19*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 20*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 21*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 22*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 23*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 24*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 25*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 26*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 27*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 28*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 29*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 30*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 31*64(%[addr]),   %%zmm1 \n"

#define READ_NT_4096_ASM \
    "vmovntdqa 0*64(%[addr]),   %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 4*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 5*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 6*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 7*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 8*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 9*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 10*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 11*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 12*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 13*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 14*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 15*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 16*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 17*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 18*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 19*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 20*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 21*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 22*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 23*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 24*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 25*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 26*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 27*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 28*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 29*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 30*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 31*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 32*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 33*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 34*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 35*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 36*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 37*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 38*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 39*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 40*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 41*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 42*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 43*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 44*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 45*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 46*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 47*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 48*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 49*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 50*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 51*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 52*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 53*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 54*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 55*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 56*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 57*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 58*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 59*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 60*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 61*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 62*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 63*64(%[addr]),   %%zmm1 \n"

#define READ_NT_8192_ASM \
    "vmovntdqa 0*64(%[addr]),   %%zmm0 \n" \
    "vmovntdqa 1*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 2*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 3*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 4*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 5*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 6*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 7*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 8*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 9*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 10*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 11*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 12*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 13*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 14*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 15*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 16*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 17*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 18*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 19*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 20*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 21*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 22*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 23*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 24*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 25*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 26*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 27*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 28*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 29*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 30*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 31*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 32*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 33*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 34*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 35*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 36*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 37*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 38*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 39*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 40*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 41*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 42*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 43*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 44*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 45*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 46*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 47*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 48*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 49*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 50*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 51*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 52*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 53*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 54*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 55*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 56*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 57*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 58*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 59*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 60*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 61*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 62*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 63*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 64*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 65*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 66*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 67*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 68*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 69*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 70*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 71*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 72*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 73*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 74*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 75*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 76*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 77*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 78*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 79*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 80*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 81*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 82*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 83*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 84*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 85*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 86*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 87*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 88*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 89*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 90*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 91*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 92*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 93*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 94*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 95*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 96*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 97*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 98*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 99*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 100*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 101*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 102*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 103*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 104*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 105*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 106*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 107*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 108*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 109*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 110*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 111*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 112*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 113*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 114*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 115*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 116*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 117*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 118*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 119*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 120*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 121*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 122*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 123*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 124*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 125*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 126*64(%[addr]),   %%zmm1 \n" \
    "vmovntdqa 127*64(%[addr]),   %%zmm1 \n"

#define WRITE_NT_64_ASM \
    "vmovntdq %%zmm0, 0(%[addr]) \n"\

#define WRITE_NT_128_ASM \
    "vmovntdq %%zmm0, 0(%[addr]) \n"\
    "vmovntdq %%zmm0, 1*64(%[addr]) \n"

#define WRITE_NT_256_ASM \
    "vmovntdq %%zmm0, 0(%[addr])     \n" \
    "vmovntdq %%zmm0, 1*64(%[addr])    \n" \
    "vmovntdq %%zmm0, 2*64(%[addr])    \n" \
    "vmovntdq %%zmm0, 3*64(%[addr])    \n"

#define WRITE_NT_512_ASM \
    "vmovntdq %%zmm0, 0*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 1*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 2*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 3*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 4*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 5*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 6*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 7*64(%[addr]) \n" 

#define WRITE_NT_1024_ASM \
    "vmovntdq   %%zmm0, 0*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 1*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 2*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 3*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 4*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 5*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 6*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 7*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 8*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 9*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 10*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 11*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 12*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 13*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 14*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 15*64(%[addr]) \n" \

#define WRITE_NT_2048_ASM \
    "vmovntdq   %%zmm0, 0*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 1*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 2*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 3*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 4*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 5*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 6*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 7*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 8*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 9*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 10*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 11*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 12*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 13*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 14*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 15*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 16*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 17*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 18*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 19*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 20*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 21*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 22*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 23*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 24*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 25*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 26*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 27*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 28*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 29*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 30*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 31*64(%[addr]) \n"

#define WRITE_NT_4096_ASM \
    "vmovntdq   %%zmm0, 0*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 1*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 2*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 3*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 4*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 5*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 6*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 7*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 8*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 9*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 10*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 11*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 12*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 13*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 14*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 15*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 16*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 17*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 18*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 19*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 20*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 21*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 22*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 23*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 24*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 25*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 26*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 27*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 28*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 29*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 30*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 31*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 32*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 33*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 34*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 35*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 36*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 37*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 38*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 39*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 40*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 41*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 42*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 43*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 44*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 45*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 46*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 47*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 48*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 49*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 50*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 51*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 52*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 53*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 54*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 55*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 56*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 57*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 58*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 59*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 60*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 61*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 62*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 63*64(%[addr]) \n"

#define WRITE_NT_8192_ASM \
    "vmovntdq   %%zmm0, 0*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 1*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 2*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 3*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 4*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 5*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 6*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 7*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 8*64(%[addr]) \n" \
    "vmovntdq   %%zmm0, 9*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 10*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 11*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 12*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 13*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 14*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 15*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 16*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 17*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 18*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 19*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 20*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 21*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 22*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 23*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 24*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 25*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 26*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 27*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 28*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 29*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 30*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 31*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 32*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 33*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 34*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 35*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 36*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 37*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 38*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 39*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 40*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 41*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 42*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 43*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 44*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 45*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 46*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 47*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 48*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 49*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 50*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 51*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 52*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 53*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 54*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 55*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 56*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 57*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 58*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 59*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 60*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 61*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 62*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 63*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 64*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 65*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 66*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 67*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 68*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 69*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 70*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 71*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 72*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 73*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 74*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 75*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 76*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 77*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 78*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 79*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 80*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 81*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 82*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 83*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 84*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 85*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 86*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 87*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 88*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 89*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 90*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 91*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 92*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 93*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 94*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 95*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 96*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 97*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 98*64(%[addr]) \n" \
    "vmovntdq  %%zmm0, 99*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 100*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 101*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 102*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 103*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 104*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 105*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 106*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 107*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 108*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 109*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 110*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 111*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 112*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 113*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 114*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 115*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 116*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 117*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 118*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 119*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 120*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 121*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 122*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 123*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 124*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 125*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 126*64(%[addr]) \n" \
    "vmovntdq %%zmm0, 127*64(%[addr]) \n"













