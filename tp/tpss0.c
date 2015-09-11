
  char PI_ID[MAX_SDF_LINE_LENGTH];
  char PI_NAME[MAX_SDF_LINE_LENGTH];

  char PROJECT_ID[9];
  char PROJECT_TITLE[MAX_SDF_LINE_LENGTH];
  char PROJECT_REMPI[MAX_SDF_LINE_LENGTH];
  char PROJECT_REMPO[MAX_SDF_LINE_LENGTH];

  unsigned int  SESSION_ID=0;
  char SESSION_TITLE[MAX_SDF_LINE_LENGTH];
  char SESSION_REMPI[MAX_SDF_LINE_LENGTH];
  char SESSION_REMPO[MAX_SDF_LINE_LENGTH];

  int  SESSION_CRA=0;
  signed short int  SESSION_DRX_BEAM=-1;
  char SESSION_SPC[32];

  int  SESSION_MRP_ASP=-1;
  int  SESSION_MRP_DP_=-1;
  int  SESSION_MRP_DR1=-1;
  int  SESSION_MRP_DR2=-1;
  int  SESSION_MRP_DR3=-1;
  int  SESSION_MRP_DR4=-1;
  int  SESSION_MRP_DR5=-1;
  int  SESSION_MRP_SHL=-1;
  int  SESSION_MRP_MCS=-1;

  int  SESSION_MUP_ASP=-1;
  int  SESSION_MUP_DP_=-1;
  int  SESSION_MUP_DR1=-1;
  int  SESSION_MUP_DR2=-1;
  int  SESSION_MUP_DR3=-1;
  int  SESSION_MUP_DR4=-1;
  int  SESSION_MUP_DR5=-1;
  int  SESSION_MUP_SHL=-1;
  int  SESSION_MUP_MCS=-1;

  int  SESSION_LOG_SCH=0;
  int  SESSION_LOG_EXE=0;
  int  SESSION_INC_SMIB=0;
  int  SESSION_INC_DES=0;

  /* structure containing one observation. */
  struct obs_struct {
    unsigned int OBS_ID;
    char OBS_TITLE[MAX_SDF_LINE_LENGTH];
    char OBS_TARGET[MAX_SDF_LINE_LENGTH];
    char OBS_REMPI[MAX_SDF_LINE_LENGTH];
    char OBS_REMPO[MAX_SDF_LINE_LENGTH];
    long int OBS_START_MJD;
    long int OBS_START_MPM;
    char OBS_START[MAX_SDF_NOTE_LENGTH];
    long int OBS_DUR;
    char OBS_DURp[MAX_SDF_NOTE_LENGTH]; /* this is OBS_DUR+ */
    unsigned short int OBS_MODE;
    char OBS_BDM[32];
    float OBS_RA;
    float OBS_DEC;
    int OBS_B;
    long int OBS_FREQ1;
    char OBS_FREQ1p[MAX_SDF_NOTE_LENGTH]; /* this is OBS_FREQ1+ */
    long int OBS_FREQ2;
    char OBS_FREQ2p[MAX_SDF_NOTE_LENGTH]; /* this is OBS_FREQ2+ */
    int OBS_BW;
    char OBS_BWp[MAX_SDF_NOTE_LENGTH]; /* this is OBS_BW+ */
    long int OBS_STP_N;
    int OBS_STP_RADEC;
    float OBS_STP_C1[MAX_STP_N];
    float OBS_STP_C2[MAX_STP_N];
    long int OBS_STP_T[MAX_STP_N];
    long int OBS_STP_FREQ1[MAX_STP_N];
    char OBS_STP_FREQ1p[MAX_STP_N][MAX_SDF_NOTE_LENGTH];
    long int OBS_STP_FREQ2[MAX_STP_N];
    char OBS_STP_FREQ2p[MAX_STP_N][MAX_SDF_NOTE_LENGTH];
    int OBS_STP_B[MAX_STP_N];
    unsigned short int OBS_BEAM_DELAY[MAX_STP_N][521];
    short int OBS_BEAM_GAIN[MAX_STP_N][LWA_MAX_NSTD+1][3][3];
    int OBS_FEE[LWA_MAX_NSTD+1][2];
    int OBS_ASP_FLT[LWA_MAX_NSTD+1];
    int OBS_ASP_AT1[LWA_MAX_NSTD+1];
    int OBS_ASP_AT2[LWA_MAX_NSTD+1];
    int OBS_ASP_ATS[LWA_MAX_NSTD+1];
#ifdef USE_ADP
    long int OBS_TBF_SAMPLES;
    int OBS_TBF_GAIN;
#else
    int OBS_TBW_BITS;
    long int OBS_TBW_SAMPLES;
#endif
    int OBS_TBN_GAIN;
    int OBS_DRX_GAIN;
    long int ASP_setup_time; // not part of SDF 
    };

  /* the observations, assembled as an array of "obs_struct" structures */
  static struct obs_struct obs[MAX_NUMBER_OF_OBSERVATIONS]; 

  /* additional keywords that appear only in the session specification file (and not the SDF) */
  unsigned long int SESSION_START_MJD=0;
  unsigned long int SESSION_START_MPM=0;
  unsigned long int SESSION_DUR=0;
  unsigned int SESSION_NOBS=0;

