#define WAVE_CH 2
#define WAVE_BIT 16
#define WAVE_PLANE 2
#define WAVE_RATE (44100UL)

struct wave_pcm;

struct wave_pcm{
  struct wave_pcm *next;
  unsigned int left;
  void *ptr;
  char data[4];
};

struct pcm_data {
  int len;
  void *data;
};

int sound_open(void);
void sound_close(void);
void sound_stop(void);
void sound_run(void);
void pcm_set(struct pcm_data *pd);

struct pcm_data *load_waveform(const char *filename);
