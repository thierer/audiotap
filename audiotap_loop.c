/* Audiotap: a program for playing TAP files or converting them to WAV files,
 * and to convert C64 tapes to TAP files
 *
 * Copyright (c) Fabrizio Gennari, 2011
 *
 * The program is distributed under the GNU General Public License.
 * See file LICENSE.TXT for details.
 *
 * audiotap_loop.c : core of both adio->tap and tap->audio
 * 
 * This file is shared between the audio->tap part and the tap->audio part
 * This file is part of Audiotap core processing files
 */

#include "audiotap_loop.h"
#include "audiotap.h"
#include "audiotap_callback.h"
#include <stddef.h>
#include <limits.h>

static struct audiotap *audiotap_interruptible = NULL;

void audiotap_interrupt(){
  if(audiotap_interruptible)
    audiotap_terminate(audiotap_interruptible);
}

static void update_status(struct audiotap *audiotap_in){
 int currlen, currloudness;
 currlen = audio2tap_get_current_pos(audiotap_in);
 if (currlen != -1)
   statusbar_update(currlen + 1);
 else if ( (currloudness=audio2tap_get_current_sound_level(audiotap_in)) != -1)
   statusbar_update(currloudness);
}

void audiotap_loop(struct audiotap *audiotap_in
                   ,struct audiotap *audiotap_out
                   ,struct audiotap *interruptible){
  enum audiotap_status status = AUDIOTAP_OK;
  unsigned int datalen = 0;
  int totlen;

  if ( (totlen = audio2tap_get_total_len(audiotap_in)) != -1)
    statusbar_initialize(totlen);
  else
    statusbar_initialize(INT_MAX);

  audiotap_interruptible = interruptible;
  while(status == AUDIOTAP_OK){
    uint32_t pulse, raw_pulse;

    if ((++datalen) % 10000 == 0)
      update_status(audiotap_in);
    status = audio2tap_get_pulses(audiotap_in, &pulse, &raw_pulse);
    if (status != AUDIOTAP_OK)
      break;

    status = tap2audio_set_pulse(audiotap_out, pulse);
  }
  update_status(audiotap_in);
  statusbar_exit();
  if (status == AUDIOTAP_INTERRUPTED)
    warning_message("Interrupted");
  else if(status != AUDIOTAP_OK && status != AUDIOTAP_EOF)
    error_message("Something went wrong");
  audiotap_interruptible = NULL;
  tap2audio_close(audiotap_out);
  audio2tap_close(audiotap_in);
}

