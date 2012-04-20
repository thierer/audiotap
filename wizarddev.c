#include "wav2prg_api.h"

static enum wav2prg_bool wizarddev_get_block_info(struct wav2prg_context* context, const struct wav2prg_functions* functions, struct wav2prg_plugin_conf* conf, struct wav2prg_block_info* info)
{
  uint16_t exec_address;
  uint8_t id;

  if (functions->get_byte_func(context, functions, conf, &id) == wav2prg_false)
    return wav2prg_false;
  functions->number_to_name_func(id, &info->name);
  if (functions->get_word_func(context, functions, conf, &info->start) == wav2prg_false)
      return wav2prg_false;
  if (functions->get_word_func(context, functions, conf, &info->end) == wav2prg_false)
    return wav2prg_false;
  info->end++;
  if (functions->get_word_func(context, functions, conf, &exec_address) == wav2prg_false)
    return wav2prg_false;
  return wav2prg_true;
}

static uint16_t wizarddev_thresholds[]={0x180};

static const struct wav2prg_plugin_conf wizarddev =
{
  msbf,
  wav2prg_xor_checksum,
  wav2prg_compute_and_check_checksum,
  2,
  wizarddev_thresholds,
  NULL,
  wav2prg_pilot_tone_made_of_1_bits_followed_by_0,
  2,
  0,
  NULL,
  2000,
  first_to_last,
  NULL
};

static const struct wav2prg_plugin_conf* wizarddev_get_state(void)
{
  return &wizarddev;
}
static const struct wav2prg_plugin_functions wizarddev_functions = {
    NULL,
    NULL,
    NULL,
    NULL,
    wizarddev_get_block_info,
    NULL,
    wizarddev_get_state,
    NULL,
    NULL,
    NULL,
    NULL
};

PLUGIN_ENTRY(wizarddev)
{
  register_loader_func(&wizarddev_functions, "Wizard Development");
}
