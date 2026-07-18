/****************************************************************************
 * apps/examples/picotts/picotts_cmd.c
 * Standalone PicoTTS — direct API + NuttX audio subsystem output.
 *
 * Plays back through any board that has a NuttX audio output device
 * registered under /dev/audio (i.e. CONFIG_AUDIO plus a lower-half codec
 * driver such as CS43L22, WM8776, ES8311, the sim's audio driver, etc.),
 * instead of relying on host ALSA. Follows the same
 * open -> configure -> alloc/enqueue -> start -> dequeue-loop -> stop
 * protocol used by apps/system/nxplayer.
 ****************************************************************************/

#include <nuttx/config.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <nuttx/audio/audio.h>

#include "picoapi.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Used only if the driver doesn't report a preference via
 * AUDIOIOC_GETBUFFERINFO.
 */

#define PICOTTS_AUDIO_NBUFFERS_DEFAULT   4
#define PICOTTS_AUDIO_BUFSIZE_DEFAULT    4096

/* PicoTTS always emits 16-bit mono PCM at 16kHz */

#define PICOTTS_SAMPLERATE                16000
#define PICOTTS_CHANNELS                  1
#define PICOTTS_BPSAMP                    16

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: fill_next_chunk
 *
 *   Copies the next slice of the already-synthesized PCM buffer into an
 *   audio pipeline buffer, and flags it as the final buffer in the stream
 *   once all of the PCM data has been consumed.
 *
 ****************************************************************************/

static void fill_next_chunk(FAR struct ap_buffer_s *apb,
                             FAR const uint8_t *pcm, size_t pcmlen,
                             FAR size_t *pos)
{
  size_t remain = pcmlen - *pos;
  size_t n = remain < apb->nmaxbytes ? remain : apb->nmaxbytes;

  memcpy(apb->samp, pcm + *pos, n);

  apb->nbytes  = n;
  apb->curbyte = 0;
  apb->flags   = 0;

  *pos += n;

  if (*pos >= pcmlen)
    {
      apb->flags |= AUDIO_APB_FINAL;
    }
}

/****************************************************************************
 * Name: enqueue_buffer
 ****************************************************************************/

static int enqueue_buffer(int fd, FAR struct ap_buffer_s *apb)
{
  struct audio_buf_desc_s desc;

  desc.numbytes = apb->nbytes;
  desc.u.buffer = apb;

  return ioctl(fd, AUDIOIOC_ENQUEUEBUFFER, (unsigned long)&desc);
}

/****************************************************************************
 * Name: picotts_play_pcm
 *
 *   Streams a block of 16-bit PCM audio out through the first
 *   output-capable, PCM-capable device found under /dev/audio, using the
 *   standard NuttX audio pipeline-buffer protocol (mirrors
 *   apps/system/nxplayer's playthread, simplified since the whole clip is
 *   already resident in memory).
 *
 ****************************************************************************/

static int picotts_play_pcm(FAR const uint8_t *pcm, size_t pcmlen,
                             uint32_t samplerate, uint8_t channels,
                             uint8_t bpsamp)
{
  struct audio_caps_s      caps;
  struct audio_caps_desc_s cfgdesc;
  struct ap_buffer_info_s  buf_info;
  struct audio_buf_desc_s  bufdesc;
  FAR struct ap_buffer_s **buffers = NULL;
  struct mq_attr           attr;
  char                     path[32];
  char                     mqname[32];
  FAR DIR                 *dirp;
  FAR struct dirent       *dent;
  mqd_t                    mq = (mqd_t)-1;
  int                      fd = -1;
  int                      nallocated = 0;
  int                      ret;
  size_t                   pos = 0;
  int                      i;

  /* Locate an output-capable, PCM-capable device under /dev/audio.  This
   * mirrors nxplayer_opendevice()'s search logic so it works with whatever
   * lower-half codec driver a given board has registered, rather than
   * hard-coding a device path.
   */

  dirp = opendir("/dev/audio");
  if (dirp == NULL)
    {
      printf("picotts: /dev/audio not found -- is CONFIG_AUDIO enabled "
             "and a codec driver registered?\n");
      return -ENODEV;
    }

  while ((dent = readdir(dirp)) != NULL)
    {
      snprintf(path, sizeof(path), "/dev/audio/%s", dent->d_name);

      fd = open(path, O_RDWR | O_CLOEXEC);
      if (fd < 0)
        {
          continue;
        }

      memset(&caps, 0, sizeof(caps));
      caps.ac_len     = sizeof(caps);
      caps.ac_type    = AUDIO_TYPE_QUERY;
      caps.ac_subtype = AUDIO_TYPE_QUERY;

      if (ioctl(fd, AUDIOIOC_GETCAPS, (unsigned long)&caps) == caps.ac_len &&
          (caps.ac_format.hw & (1 << (AUDIO_FMT_PCM - 1))) != 0 &&
          (caps.ac_controls.b[0] & AUDIO_TYPE_OUTPUT) != 0)
        {
          break;
        }

      close(fd);
      fd = -1;
    }

  closedir(dirp);

  if (fd < 0)
    {
      printf("picotts: no PCM-capable output audio device found under "
             "/dev/audio\n");
      return -ENODEV;
    }

  printf("picotts: using audio device %s\n", path);

  /* Reserve the device.  Not every lower-half driver implements this, so
   * tolerate ENOTTY/ENOSYS.
   */

  ret = ioctl(fd, AUDIOIOC_RESERVE, 0);
  if (ret < 0 && errno != ENOTTY && errno != ENOSYS)
    {
      printf("picotts: AUDIOIOC_RESERVE failed: %d\n", errno);
      close(fd);
      return -errno;
    }

  /* Configure for raw 16-bit PCM at the requested rate/channel count */

  memset(&cfgdesc, 0, sizeof(cfgdesc));
  cfgdesc.caps.ac_len            = sizeof(struct audio_caps_s);
  cfgdesc.caps.ac_type           = AUDIO_TYPE_OUTPUT;
  cfgdesc.caps.ac_channels       = channels;
  cfgdesc.caps.ac_subtype        = AUDIO_FMT_PCM;
  cfgdesc.caps.ac_controls.hw[0] = samplerate;
  cfgdesc.caps.ac_controls.b[2]  = bpsamp;
  cfgdesc.caps.ac_controls.b[3]  = samplerate >> 16;

  if (ioctl(fd, AUDIOIOC_CONFIGURE, (unsigned long)&cfgdesc) < 0)
    {
      printf("picotts: AUDIOIOC_CONFIGURE failed: %d\n", errno);
      ret = -errno;
      goto errout_with_fd;
    }

  /* Ask the driver how many / how big it wants its pipeline buffers;
   * fall back to sane defaults if it doesn't say.
   */

  if (ioctl(fd, AUDIOIOC_GETBUFFERINFO, (unsigned long)&buf_info) < 0)
    {
      buf_info.nbuffers    = PICOTTS_AUDIO_NBUFFERS_DEFAULT;
      buf_info.buffer_size = PICOTTS_AUDIO_BUFSIZE_DEFAULT;
    }

  buffers = calloc(buf_info.nbuffers, sizeof(FAR struct ap_buffer_s *));
  if (buffers == NULL)
    {
      ret = -ENOMEM;
      goto errout_with_fd;
    }

  /* Create + register the message queue the driver will use to tell us
   * when it has finished with a buffer (AUDIO_MSG_DEQUEUE) and when
   * playback is finished (AUDIO_MSG_COMPLETE).
   */

  snprintf(mqname, sizeof(mqname), "/tmp/picotts%d", getpid());

  attr.mq_maxmsg  = buf_info.nbuffers + 8;
  attr.mq_msgsize = sizeof(struct audio_msg_s);
  attr.mq_curmsgs = 0;
  attr.mq_flags   = 0;

  mq = mq_open(mqname, O_RDWR | O_CREAT, 0644, &attr);
  if (mq == (mqd_t)-1)
    {
      printf("picotts: mq_open failed: %d\n", errno);
      ret = -errno;
      goto errout_with_buffers;
    }

  ioctl(fd, AUDIOIOC_REGISTERMQ, (unsigned long)mq);

  /* Allocate the pipeline buffers */

  for (i = 0; i < buf_info.nbuffers; i++)
    {
      bufdesc.numbytes  = buf_info.buffer_size;
      bufdesc.u.pbuffer = &buffers[i];

      if (ioctl(fd, AUDIOIOC_ALLOCBUFFER, (unsigned long)&bufdesc) !=
          sizeof(bufdesc))
        {
          printf("picotts: AUDIOIOC_ALLOCBUFFER failed for buffer %d\n", i);
          ret = -EIO;
          goto errout_with_mq;
        }

      nallocated++;
    }

  /* Prime the pipeline with as many buffers as we have data for */

  for (i = 0; i < nallocated && pos < pcmlen; i++)
    {
      fill_next_chunk(buffers[i], pcm, pcmlen, &pos);

      if (enqueue_buffer(fd, buffers[i]) < 0)
        {
          printf("picotts: AUDIOIOC_ENQUEUEBUFFER failed: %d\n", errno);
          ret = -errno;
          goto errout_with_mq;
        }
    }

  if (ioctl(fd, AUDIOIOC_START, 0) < 0)
    {
      printf("picotts: AUDIOIOC_START failed: %d\n", errno);
      ret = -errno;
      goto errout_with_mq;
    }

  /* Service dequeue notifications, refilling/re-enqueuing buffers with the
   * remainder of the synthesized clip, until the driver reports playback
   * complete.
   */

  for (; ; )
    {
      struct audio_msg_s msg;
      unsigned int prio;

      if (mq_receive(mq, (FAR char *)&msg, sizeof(msg), &prio) !=
          sizeof(msg))
        {
          continue;
        }

      if (msg.msg_id == AUDIO_MSG_DEQUEUE)
        {
          FAR struct ap_buffer_s *apb = (FAR struct ap_buffer_s *)msg.u.ptr;

          if (pos < pcmlen)
            {
              fill_next_chunk(apb, pcm, pcmlen, &pos);
              enqueue_buffer(fd, apb);
            }
        }
      else if (msg.msg_id == AUDIO_MSG_COMPLETE)
        {
          ioctl(fd, AUDIOIOC_STOP, 0);
          break;
        }
    }

  ret = OK;

errout_with_mq:
  for (i = 0; i < nallocated; i++)
    {
      bufdesc.u.buffer = buffers[i];
      ioctl(fd, AUDIOIOC_FREEBUFFER, (unsigned long)&bufdesc);
    }

  ioctl(fd, AUDIOIOC_UNREGISTERMQ, (unsigned long)mq);
  mq_close(mq);
  mq_unlink(mqname);

errout_with_buffers:
  free(buffers);

errout_with_fd:
  ioctl(fd, AUDIOIOC_RELEASE, 0);
  close(fd);

  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

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
  int ret;

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

  /* Write raw PCM to hostfs (handy for the sim / off-target inspection) */
  {
    int fd = open("/data/picotts_out.raw", O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if (fd >= 0) { write(fd, buffer, bufused); close(fd); }
  }

  /* Play it out through the NuttX audio subsystem */

  printf("picotts: synthesized %u bytes of PCM, playing...\n", bufused);

  ret = picotts_play_pcm((FAR const uint8_t *)buffer, bufused,
                          PICOTTS_SAMPLERATE, PICOTTS_CHANNELS,
                          PICOTTS_BPSAMP);
  if (ret < 0)
    {
      printf("picotts: playback failed: %d\n", ret);
    }

  free(buffer);
  return (ret < 0) ? 1 : 0;
}
