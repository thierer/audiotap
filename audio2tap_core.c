/* Audiotap: a program for playing TAP files or converting them to WAV files,
 * and to convert C64 tapes to TAP files
 *
 * Copyright (c) Fabrizio Gennari, 2003
 *
 * The program is distributed under the GNU General Public License.
 * See file LICENSE.TXT for details.
 *
 * audio2tap_core.c : main audio->tap processing
 *
 * This file belongs to the audio->tap part
 * This file is part of Audiotap core processing files
 */

#include <errno.h>
#include <string.h>
#include "audiotap_callback.h"
#include "audio2tap_core.h"
#include "audiotap_loop.h"
#include "audiotap.h"

void audio2tap(char *infile,
          char *outfile,
          uint32_t freq,
          struct tapenc_params *params,
          uint8_t tap_version,
          uint8_t clock,
          uint8_t videotype
)
{
  uint8_t machine, semiwaves;
  struct audiotap *audiotap_in, *audiotap_out;

  if (tap_version > 1){
    error_message("TAP version %u is unsupported", infile);
    return;
  }

  switch(clock){
  default:
    machine=TAP_MACHINE_C64;
    break;
  case 1:
    machine=TAP_MACHINE_VIC;
    break;
  case 2:
    machine=TAP_MACHINE_C16;
    break;
  case 3:
    machine=TAP_MACHINE_C16;
    params->inverted = TAP_TRIGGER_ON_BOTH_EDGES;
    tap_version = 2;
    break;
  }

  if (infile && strlen(infile)){
    if(audio2tap_open_from_file(&audiotap_in,
                                infile,
                                params,
                                &machine,
                                &videotype,
                                &semiwaves) != AUDIOTAP_OK){
      error_message("File %s does not exist, is not a supported audio file, or cannot be opened for some reasons", infile);
      return;
    }
  }
  else if (audio2tap_from_soundcard(&audiotap_in,
                                    freq,
                                    params,
                                    machine,
                                    videotype) != AUDIOTAP_OK){
    error_message("Sound card cannot be opened");
    return;
  }

  if (tap2audio_open_to_tapfile(&audiotap_out, outfile, tap_version,
                                machine,
                                videotype) != AUDIOTAP_OK){
    error_message("Cannot open file %s: %s", outfile, strerror(errno));
    audio2tap_close(audiotap_in);
    return;
  }

  audiotap_loop(audiotap_in, audiotap_out, audiotap_in);
}
