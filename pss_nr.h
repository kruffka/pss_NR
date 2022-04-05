
#ifdef DEFINE_VARIABLES_PSS_NR_H
#define EXTERN
#else
#define EXTERN  extern
#endif









/* profiling structure */
EXTERN time_stats_t generic_time[TIME_LAST];

int pss_synchro_nr(PHY_VARS_NR_UE *PHY_vars_UE, int is, int rate_change);