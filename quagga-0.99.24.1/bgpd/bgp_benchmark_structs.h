#ifndef BGPD_BGPBENCHMARKSTRUCTS_H
#define BGPD_BGPBENCHMARKSTRUCTS_H

// DBGP BENCHMARK Holds data regarding collecting statistics for benchmarks

// generic timer.
struct BenchmarkTimer {
  struct timespec start_time; // the start time of the measurement
  struct timespec end_time; // The time when the benchmark ended
};

// Information for capturing average processing latency (bgp_update_main)
struct ProcessingLatency {
  // timer for the bgp_update_main latency endtime might not be used
  struct BenchmarkTimer bgp_update_main_timer;
  // the total duration measured for one particular advert. This is necessary
  // for when the processing work is spread out amongst different types of delay
  // (deserialization, lookup service). This value is added to total_durations
  // when there is no more processing latency necessary to measure
  int64_t current_duration;
  int64_t total_durations; // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements; // The number of measurements made
};

struct EndToEndLatency {
  // timer for the endtoend (bgp_read to end of bgp_update_main) latency
  // endtime might not be used
  struct BenchmarkTimer bgp_end_to_end_timer;
  // True if bgp_read set this. Therefore, we can update the total_durations.
  // This is because, it seems quagga batch reads packets so a start time is not
  // updated every time.
  int moved;
  int64_t total_durations; // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements; // The number of measurements made
};

// structure to capture statistics for deserialization latency
struct DeserializationLatency {
  // timer for the traditional bgp deserialization. May not hold end_time
  struct BenchmarkTimer bgp_deserialization_timer;
  struct BenchmarkTimer bgp_deserialization_beagle_timer;
  // True if bgp_read set this. Therefore, we can update the total_durations.
  // This is because, it seems quagga batch reads packets so a start time is not
  // updated every time.
  int moved;
  // the total duration measured for one particular advert. This is necessary
  // for when the deserialization work is spread out (one in the traditional bgp
  // deserialization and one for when we deserialized the IA). This value is
  // added to total_durations when there is no more deserialization latency
  // necessary to measure
  int64_t current_duration;
  int64_t total_durations_bgp_deserialization; // the sum total of all durations measured (nanoseconds) for the bgp deserialization part
  int64_t num_measurements_bgp_deserialization; // The number of measurements made for the trraditional bgp deserializatino part
  int64_t total_durations_bgp_deserialization_beagle; // the sum total of all durations measured (nanoseconds) for beagle
  int64_t num_measurements_bgp_deserialization_beagle; // The number of measurements made for beagle deserialization
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
  int64_t total_durations; // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements; // The number of measurements made
  
};

struct BgpBenchmarkStats {
  uint64_t advertisements_seen; // Number of advertisements seen during run
  struct timespec start_time; // The time when we started to receive advertisements
  struct timespec end_time; // The time when the benchmark ended
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

void UpdateDeserializationCurrentDuration(struct DeserializationLatency* deserialization_latency);

void UpdateLookupServiceCurrentDuration(struct LookUpServiceLatency* lookupservice_latency);

extern void UpdateProcessingCurrentDuration(struct ProcessingLatency* processing_latency);

#endif
