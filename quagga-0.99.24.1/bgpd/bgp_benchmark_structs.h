#ifndef BGPD_BGPBENCHMARKSTRUCTS_H
#define BGPD_BGPBENCHMARKSTRUCTS_H

// DBGP BENCHMARK Holds data regarding collecting statistics for benchmarks

// generic timer.
struct BenchmarkTimer {
  struct timespec start_time;  // the start time of the measurement
  struct timespec end_time;    // The time when the benchmark ended
};

// Structure to hold contiguous stats for a funciton. For example, you could
// have one of these keeping track of processing in one thread nlri_parse and
// another in bgp_process_main.
struct ContiguousStats {
  struct BenchmarkTimer timer;
  int64_t total_durations;
  int64_t num_measurements;
  // holds the current duration, in the case where the stats needs to be
  // separated out (e.g. measuring deseraialization and there is alookup service
  // call)
  int64_t current_duration;
  // optional boolean where the stats you want to measure are separated between
  // functions. Comments should say when this is expected to be used.
  int32_t contiguous_separation;
};

// Information for capturing average processing latency (bgp_update_main)
struct ProcessingLatency {
  struct ContiguousStats
      bgp_nlri_parse_stats;  // processing latency for bgp_nlri-parse
  struct ContiguousStats
      bgp_process_main_stats;  // processing latency for bgp_process_main
};

struct EndToEndLatency {
  // timer for the endtoend (bgp_read to end of bgp_update_main) latency
  // endtime might not be used
  struct BenchmarkTimer bgp_end_to_end_timer;
  // True if bgp_read set this. Therefore, we can update the total_durations.
  // This is because, it seems quagga batch reads packets so a start time is not
  // updated every time.
  int moved;
  int64_t
      total_durations;  // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements;  // The number of measurements made
};

// structure to capture statistics for deserialization latency
struct DeserializationLatency {
  // timer for the traditional bgp deserialization. May not hold end_time
  struct ContiguousStats bgp_deserialization_stats;
  struct ContiguousStats bgp_deserialization_beagle_stats;
};

// structure to capture statistics for lookup service latency
struct LookUpServiceLatency {
  // timer for the traditional bgp deserialization. May not hold end_time
  struct BenchmarkTimer bgp_lookupservice_timer;
  // the total duration measured for one particular advert. This is necessary
  // for when the lookupservice work is spread out (lookup serivce calls are
  // made multiple times). This value is added to total_durations when there is
  // no more lookuplatency latency necessary to measure
  int64_t current_duration;
  int64_t
      total_durations;  // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements;  // The number of measurements made
};

struct BgpBenchmarkStats {
  uint64_t advertisements_seen;  // Number of advertisements seen during run
  u_int32_t started_advert_timer; // 1 if we have received the first advert (so
                                  // we don't update the start_time for
                                  // advertisement tput calculation)
  struct timespec
      start_time;  // The time when we started to receive advertisements
  struct timespec end_time;  // The time when the benchmark ended
  // holds stats for the deserialization latency
  struct DeserializationLatency deserialization_latency;
  // holds stats for the processing latency
  struct ProcessingLatency processing_latency;
  // holds stats for the lookup service latency
  struct LookUpServiceLatency lookup_service_latency;
  // holds stats for the end to end latency
  struct EndToEndLatency end_to_end_latency;
};

typedef struct BgpBenchmarkStats* BgpBenchmarkStatsPtr;

int64_t GetNanoSecDuration(struct timespec start_time);

void PrintBenchmarkStats(struct BgpBenchmarkStats bgp_benchmark_stats);

/* void UpdateDeserializationCurrentDuration( */
/*     struct DeserializationLatency* deserialization_latency); */

void UpdateLookupServiceCurrentDuration(
    struct LookUpServiceLatency* lookupservice_latency);

/* extern void UpdateProcessingCurrentDuration( */
/*     struct ProcessingLatency* processing_latency); */
void UpdateContiguousStatsDuration(struct ContiguousStats* contiguous_stats) ;
void StartContiguousStatsDuration(struct ContiguousStats * contiguous_stats);
void EndContiguousStatsMeasurement(struct ContiguousStats * contiguous_stats);

void InitializeContiguousStruct(struct ContiguousStats * contiguous_stats);

#endif
