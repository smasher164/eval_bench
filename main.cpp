#define PICOBENCH_IMPLEMENT
#include <cstdio>
#include <cstdlib>
#include "picobench.hpp"

// interp_switch and interp_cgoto are from Eli Bendersky's
// blog post "Computed goto for efficient dispatch tables"
// Link: https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
// Eli's blog code is licensed via the Unlicense, as mentioned in this repo:
// https://github.com/eliben/code-for-blog

#define OP_HALT     0x0
#define OP_INC      0x1
#define OP_DEC      0x2
#define OP_MUL2     0x3
#define OP_DIV2     0x4
#define OP_ADD7     0x5
#define OP_NEG      0x6

int interp_switch(unsigned char* code, int initval) {
    int pc = 0;
    int val = initval;

    while (1) {
        switch (code[pc++]) {
            default:
                return val;
            case OP_INC:
                val++;
                break;
            case OP_DEC:
                val--;
                break;
            case OP_MUL2:
                val *= 2;
                break;
            case OP_DIV2:
                val /= 2;
                break;
            case OP_ADD7:
                val += 7;
                break;
            case OP_NEG:
                val = -val;
                break;
        }
    }
}

int interp_cgoto(unsigned char* code, int initval) {
    static void* dispatch_table[] = {
        &&do_halt, &&do_inc, &&do_dec, &&do_mul2,
        &&do_div2, &&do_add7, &&do_neg};
    #define DISPATCH() goto *dispatch_table[code[pc++]]

    int pc = 0;
    int val = initval;

    DISPATCH();
        do_halt:
            return val;
        do_inc:
            val++;
            DISPATCH();
        do_dec:
            val--;
            DISPATCH();
        do_mul2:
            val *= 2;
            DISPATCH();
        do_div2:
            val /= 2;
            DISPATCH();
        do_add7:
            val += 7;
            DISPATCH();
        do_neg:
            val = -val;
            DISPATCH();
    #undef DISPATCH
}

int interp_goto(unsigned char* code, int initval) {
    #define DISPATCH() ({switch (code[pc++]) { \
    default: goto do_halt; \
    case OP_INC: goto do_inc; \
    case OP_DEC: goto do_dec; \
    case OP_MUL2: goto do_mul2; \
    case OP_DIV2: goto do_div2; \
    case OP_ADD7: goto do_add7; \
    case OP_NEG: goto do_neg; \
    }})

    int pc = 0;
    int val = initval;

    DISPATCH();
    while (1) {
        do_halt:
            return val;
        do_inc:
            val++;
            DISPATCH();
        do_dec:
            val--;
            DISPATCH();
        do_mul2:
            val *= 2;
            DISPATCH();
        do_div2:
            val /= 2;
            DISPATCH();
        do_add7:
            val += 7;
            DISPATCH();
        do_neg:
            val = -val;
            DISPATCH();
    }
    #undef DISPATCH
}

int interp_tailcall(unsigned char* code, int initval);

int fn_halt(unsigned char* code, int val) {
    return val;
}

int fn_inc(unsigned char* code, int val) {
    return interp_tailcall(code, val+1);
}

int fn_dec(unsigned char* code, int val) {
    return interp_tailcall(code, val-1);
}

int fn_mul2(unsigned char* code, int val) {
    return interp_tailcall(code, val*2);
}

int fn_div2(unsigned char* code, int val) {
    return interp_tailcall(code, val/2);
}

int fn_add7(unsigned char* code, int val) {
    return interp_tailcall(code, val+7);
}

int fn_neg(unsigned char* code, int val) {
    return interp_tailcall(code, -val);
}

int interp_tailcall(unsigned char* code, int initval) {
    unsigned char* next = code+1;
    return ((int (*)(unsigned char *, int))((char*)fn_halt+(*code)))(next, initval);
}

//unsigned char code[] = { OP_INC, OP_DEC, OP_MUL2, OP_DIV2, OP_ADD7, OP_NEG, OP_NEG, OP_DIV2, OP_DEC, OP_ADD7, OP_MUL2, OP_INC, OP_HALT };
unsigned char* code;
int initval = 42;

static void bench_interp_switch(picobench::state& s) {
	int sum = 0;
	for (auto _ : s) {
		sum += interp_switch(code, initval);
	}
	s.set_result(sum);
}
PICOBENCH(bench_interp_switch);

static void bench_interp_cgoto(picobench::state& s) {
	int sum = 0;
	for (auto _ : s) {
		sum += interp_cgoto(code, initval);
	}
	s.set_result(sum);
}
PICOBENCH(bench_interp_cgoto);

static void bench_interp_goto(picobench::state& s) {
	int sum = 0;
	for (auto _ : s) {
		sum += interp_goto(code, initval);
	}
	s.set_result(sum);
}
PICOBENCH(bench_interp_goto);

static void bench_interp_tailcall(picobench::state& s) {
	int sum = 0;
	for (auto _ : s) {
		sum += interp_goto(code, initval);
	}
	s.set_result(sum);
}
PICOBENCH(bench_interp_tailcall);

#define DSIZE 100000
unsigned char* prepare_data() {
  unsigned char* data = (unsigned char*)malloc(DSIZE);
  FILE* f = fopen("zz.bin", "rb");
  if (!f)
    return 0;
  if (fread(data, 1, DSIZE, f) != DSIZE)
    return 0;
  data[DSIZE - 1] = '\x00';
  return data;
}

int main(int argc, const char* argv[]) {
    code = prepare_data();
    picobench::runner runner;
    runner.parse_cmd_line(argc, argv);
    return runner.run();
}
