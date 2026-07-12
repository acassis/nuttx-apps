/****************************************************************************
 * apps/examples/picotts/picotts_cmd.c
 * Standalone PicoTTS — direct API + ALSA output.
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "picoapi.h"

/* ALSA host functions (sim links with libasound) */
extern int  snd_pcm_open(void **p, const char *n, int d, int m);
extern int  snd_pcm_close(void *p);
extern int  snd_pcm_drain(void *p);
extern long snd_pcm_writei(void *p, const void *b, unsigned long n);
extern int  snd_pcm_hw_params_malloc(void **h);
extern int  snd_pcm_hw_params_any(void *p, void *h);
extern int  snd_pcm_hw_params_set_access(void *p, void *h, int v);
extern int  snd_pcm_hw_params_set_format(void *p, void *h, int v);
extern int  snd_pcm_hw_params_set_channels(void *p, void *h, unsigned v);
extern int  snd_pcm_hw_params_set_rate_near(void *p, void *h, unsigned *r, int *d);
extern int  snd_pcm_hw_params(void *p, void *h);
extern void snd_pcm_hw_params_free(void *h);

int picotts_main(int argc, char *argv[])
{
  static pico_System sys;
  static pico_Resource ta, sg;
  static pico_Engine eng;
  static bool initd = false;
  pico_Char name[256];
  pico_Int16 bs, br, dt, st;
  short outbuf[2048];
  int16_t *buffer;
  unsigned bufused = 0;

  if (argc < 2) { printf("usage: picotts <text>\n"); return 1; }

  /* One-time engine init */
  if (!initd)
    {
      if (pico_initialize(malloc(3*1024*1024), 3*1024*1024, &sys))
        { printf("init fail\n"); return 1; }

      if (pico_loadResource(sys,(pico_Char*)CONFIG_PICOTTS_TA_RESOURCE,&ta))
        { printf("TA fail\n"); return 1; }

      if (pico_loadResource(sys,(pico_Char*)CONFIG_PICOTTS_SG_RESOURCE,&sg))
        { printf("SG fail\n"); return 1; }

      if (pico_createVoiceDefinition(sys,(pico_Char*)"PicoVoice"))
        { printf("voice fail\n"); return 1; }

      pico_getResourceName(sys, ta, (pico_Char*)name);
      pico_addResourceToVoiceDefinition(sys,(pico_Char*)"PicoVoice",name);

      pico_getResourceName(sys, sg, (pico_Char*)name);
      pico_addResourceToVoiceDefinition(sys,(pico_Char*)"PicoVoice",name);

      if (pico_newEngine(sys,(pico_Char*)"PicoVoice",&eng))
        { printf("engine fail\n"); return 1; }

      initd = true;
    }

  buffer = malloc(200000);
  if (!buffer) { printf("mem fail\n"); return 1; }

  /* Exact pico2wave: text + NULL, interleaved putTextUtf8/getData while BUSY */
  {
    const pico_Char *inp = (const pico_Char *)argv[1];
    pico_Int16 remaining = (pico_Int16)(strlen(argv[1]) + 1);

    while (remaining > 0)
      {
        bs = 0;
        st = pico_putTextUtf8(eng, inp, remaining, &bs);
        if (st) break;
        inp += bs; remaining -= bs;

        do
          {
            dt = br = 0;
            st = pico_getData(eng, outbuf, sizeof(outbuf), &br, &dt);
            if (br && bufused + br < 200000)
              {
                memcpy((uint8_t*)buffer + bufused, outbuf, br);
                bufused += br;
              }
          }
        while (st == 201); /* PICO_STEP_BUSY */
      }
  }

  if (bufused == 0) { printf("no audio\n"); free(buffer); return 1; }

  /* Write raw PCM to hostfs */
  {
    int fd = open("/data/picotts_out.raw", O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if (fd >= 0) { write(fd, buffer, bufused); close(fd); }
  }

  /* ALSA output */
  {
    void *pcm = NULL, *hw = NULL;
    unsigned rate = 16000;

    if (!snd_pcm_open(&pcm,"default",0,0))
      {
        snd_pcm_hw_params_malloc(&hw);
        snd_pcm_hw_params_any(pcm, hw);
        snd_pcm_hw_params_set_access(pcm, hw, 3);
        snd_pcm_hw_params_set_format(pcm, hw, 2);
        snd_pcm_hw_params_set_channels(pcm, hw, 1);
        snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0);
        snd_pcm_hw_params(pcm, hw);
        snd_pcm_hw_params_free(hw);
        snd_pcm_writei(pcm, buffer, bufused / sizeof(int16_t));
        snd_pcm_drain(pcm);
        snd_pcm_close(pcm);
      }
  }

  free(buffer);
  return 0;
}
