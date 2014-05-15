#ifndef __STUB_INTERRUPT_H
#define __STUB_INTERRUPT_H

// interrupt service routine
#define ISR(vector, ...)   \
  void isr_ ## vector (void) __attribute__ ((used, externally_visible)); \
  void isr_ ## vector (void)

#endif /* __STUB_INTERRUPT_H */
