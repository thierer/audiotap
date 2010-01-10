#include "wav2prg_api.h"

static enum wav2prg_return_values turbotape_get_block_info(struct wav2prg_context* context, const struct wav2prg_functions* functions, struct wav2prg_plugin_conf* conf, char* name, uint16_t* start, uint16_t* end)
{
  uint8_t byte;
  int i;
  if (functions->get_byte_func(context, functions, conf, &byte) == wav2prg_invalid)
    return wav2prg_invalid;
  if (byte != 1 && byte != 2 && byte != 0x61)
    return wav2prg_invalid;
  if (functions->get_word_func(context, functions, conf, start) == wav2prg_invalid)
    return wav2prg_invalid;
  if (functions->get_word_func(context, functions, conf, end) == wav2prg_invalid)
    return wav2prg_invalid;
  if (functions->get_byte_func(context, functions, conf, &byte) == wav2prg_invalid)
    return wav2prg_invalid;
  for(i=0;i<16;i++){
    if (functions->get_byte_func(context, functions, conf, name + i) != 0)
      return wav2prg_invalid;
  }
  if (functions->get_sync(context, functions, conf) == wav2prg_invalid)
    return wav2prg_invalid;
  if (functions->get_byte_func(context, functions, conf, &byte) == wav2prg_invalid)
    return wav2prg_invalid;
  return wav2prg_ok;
}

static uint16_t turbotape_thresholds[]={263};
static uint16_t turbotape_ideal_pulse_lengths[]={224, 336};
static uint8_t turbotape_pilot_sequence[]={9,8,7,6,5,4,3,2,1};

static const struct wav2prg_plugin_conf turbotape =
{
  msbf,
  wav2prg_xor_checksum,
  2,
  turbotape_thresholds,
  turbotape_ideal_pulse_lengths,
  wav2prg_synconbyte,
  2,
  9,
  turbotape_pilot_sequence,
  NULL
};

static const struct wav2prg_plugin_conf* turbotape_get_state(void)
{
  return &turbotape;
}

static const struct wav2prg_plugin_functions turbotape_functions = {
    NULL,
    NULL,
    NULL,
    turbotape_get_block_info,
    NULL,
    turbotape_get_state,
    NULL,
    NULL
};

PLUGIN_ENTRY(turbotape)
{
  register_loader_func(&turbotape_functions, "Turbo Tape 64");
}