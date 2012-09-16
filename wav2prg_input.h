#ifndef WAV2PRG_INPUT_H
#define WAV2PRG_INPUT_H

#include "wavprg_types.h"
#include <stdint.h>

struct wav2prg_input_object {
  void* object;
};

struct wav2prg_input_functions {
  int32_t           (*get_pos  )(struct wav2prg_input_object *object);
  uint8_t           (*set_pos  )(struct wav2prg_input_object *object, uint32_t pos);
  enum wav2prg_bool (*get_pulse)(struct wav2prg_input_object *object, uint32_t* pulse);
  enum wav2prg_bool (*is_eof   )(struct wav2prg_input_object *object);
  void              (*invert   )(struct wav2prg_input_object *object);
  void              (*close    )(struct wav2prg_input_object *object);
};

#endif

