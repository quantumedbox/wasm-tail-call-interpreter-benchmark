#include <stdint.h>

// Experiment on getting this technique to wasm platform.
// https://mort.coffee/home/fast-interpreters/
// It is apparent that implementation of interpreter on top of another
// introduces non-trivial overhead, reduction of which is of potential value.

union instr {
  int32_t (*fn)(union instr *instrs, int32_t *stackptr);
  int32_t operand;
};

#define POP() (*(--stackptr))
#define PUSH(val) (*(stackptr++) = (val))
#define STACK(offset) (*(stackptr - 1 - offset))
#define OPERAND() (instrs[1].operand)

static int32_t op_constant(union instr *instrs, int32_t *stackptr) {
  PUSH(OPERAND());
  __attribute__((musttail)) return instrs[2].fn(&instrs[2], stackptr);
}

static int32_t op_add(union instr *instrs, int32_t *stackptr) {
  int32_t b = POP();
  int32_t a = POP();
  PUSH(a + b);
  __attribute__((musttail)) return instrs[1].fn(&instrs[1], stackptr);
}

static int32_t op_print(union instr *instrs, int32_t *stackptr) {
  int32_t a = POP();
  // printf("%i\n", (int)a);
  __attribute__((musttail)) return instrs[1].fn(&instrs[1], stackptr);
}

static int32_t op_discard(union instr *instrs, int32_t *stackptr) {
  POP();
  __attribute__((musttail)) return instrs[1].fn(&instrs[1], stackptr);
}

static int32_t op_get(union instr *instrs, int32_t *stackptr) {
  int32_t a = STACK(OPERAND());
  PUSH(a);
  __attribute__((musttail)) return instrs[2].fn(&instrs[2], stackptr);
}

static int32_t op_set(union instr *instrs, int32_t *stackptr) {
  int32_t a = POP();
  STACK(OPERAND()) = a;
  __attribute__((musttail)) return instrs[2].fn(&instrs[2], stackptr);
}

static int32_t op_cmp(union instr *instrs, int32_t *stackptr) {
  int32_t b = POP();
  int32_t a = POP();
  if (a > b)
    PUSH(1);
  else if (a < b)
    PUSH(-1);
  else
    PUSH(0);
  __attribute__((musttail)) return instrs[1].fn(&instrs[1], stackptr);
}

static int32_t op_jgt(union instr *instrs, int32_t *stackptr) {
  int32_t a = POP();
  if (a > 0)
    instrs += instrs[1].operand;
  else
    instrs += 2;
  __attribute__((musttail)) return instrs[0].fn(&instrs[0], stackptr);
}

static int32_t op_halt(union instr *instrs, int32_t *stackptr) {
  return *stackptr;
}

int32_t main(void) {
  union instr program[] = {
      {.fn = op_constant}, {.operand = 0},

      {.fn = op_get},      {.operand = 0}, {.fn = op_get},      {.operand = 3},
      {.fn = op_add},      {.fn = op_set}, {.operand = 0},

      {.fn = op_get},      {.operand = 1}, {.fn = op_constant}, {.operand = -1},
      {.fn = op_add},      {.fn = op_set}, {.operand = 1},

      {.fn = op_get},      {.operand = 1}, {.fn = op_constant}, {.operand = 0},
      {.fn = op_cmp},      {.fn = op_jgt}, {.operand = -19},

      {.fn = op_get},      {.operand = 0}, {.fn = op_print},

      {.fn = op_halt},
  };

  int32_t stack[128] = {1, 100000000};
  return program[0].fn(program, &stack[2]);
}
