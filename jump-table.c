#include <stdint.h>

enum op {
  OP_CONSTANT,
  OP_ADD,
  OP_PRINT,
  OP_DISCARD,
  OP_GET,
  OP_SET,
  OP_CMP,
  OP_JGT,
  OP_HALT,
};

int32_t interpret(unsigned char *bytecode) {
  int32_t stack[128] = {1, 100000000};
  int32_t *stackptr = &stack[2];
  unsigned char *instrptr = bytecode;

#define POP() (*(--stackptr))
#define PUSH(val) (*(stackptr++) = (val))
#define STACK(offset) (*(stackptr - 1 - offset))
#define OPERAND()                                                              \
  (((int32_t)instrptr[1] << 0) | ((int32_t)instrptr[2] << 8) |                 \
   ((int32_t)instrptr[3] << 16) | ((int32_t)instrptr[4] << 24))

  // Note: This jump table must be synchronized with the 'enum op',
  // so that `jmptable[op]` represents the label with the code for the
  // instruction 'op'
  void *jmptable[] = {
      &&case_constant, &&case_add, &&case_print, &&case_discard, &&case_get,
      &&case_set,      &&case_cmp, &&case_jgt,   &&case_halt,
  };

  int32_t a, b;
  goto *jmptable[*instrptr];

case_constant:
  PUSH(OPERAND());
  instrptr += 5;
  goto *jmptable[*instrptr];
case_add:
  b = POP();
  a = POP();
  PUSH(a + b);
  instrptr += 1;
  goto *jmptable[*instrptr];
case_print:
  a = POP();
  // printf("%i\n", (int)a);
  instrptr += 1;
  goto *jmptable[*instrptr];
case_discard:
  POP();
  instrptr += 1;
  goto *jmptable[*instrptr];
case_get:
  a = STACK(OPERAND());
  PUSH(a);
  instrptr += 5;
  goto *jmptable[*instrptr];
case_set:
  a = POP();
  STACK(OPERAND()) = a;
  instrptr += 5;
  goto *jmptable[*instrptr];
case_cmp:
  b = POP();
  a = POP();
  if (a > b)
    PUSH(1);
  else if (a < b)
    PUSH(-1);
  else
    PUSH(0);
  instrptr += 1;
  goto *jmptable[*instrptr];
case_jgt:
  a = POP();
  if (a > 0)
    instrptr += OPERAND();
  else
    instrptr += 5;
  goto *jmptable[*instrptr];
case_halt:
  return *stackptr;
}

int32_t main(void) {
  unsigned char program[] = {
      OP_CONSTANT,
      0,
      0,
      0,
      0,

      OP_GET,
      0,
      0,
      0,
      0,
      OP_GET,
      3,
      0,
      0,
      0,
      OP_ADD,
      OP_SET,
      0,
      0,
      0,
      0,

      OP_GET,
      1,
      0,
      0,
      0,
      OP_CONSTANT,
      0xff,
      0xff,
      0xff,
      0xff, // -1 32-bit little-endian (two's
            // complement)
      OP_ADD,
      OP_SET,
      1,
      0,
      0,
      0,

      OP_GET,
      1,
      0,
      0,
      0,
      OP_CONSTANT,
      0,
      0,
      0,
      0,
      OP_CMP,
      OP_JGT,
      0xd5,
      0xff,
      0xff,
      0xff, // -43 in 32-bit little-endian (two's
            // complement)

      OP_GET,
      0,
      0,
      0,
      0,
      OP_PRINT,

      OP_HALT,
  };
  return interpret(program);
}
