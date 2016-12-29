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
  int64_t total_durations; // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements; // The number of measurements made
};

struct EndToEndLatency {
  // timer for the endtoend (bgp_read to end of bgp_update_main) latency
  // endtime might not be used
  struct BenchmarkTimer bgp_end_to_end_timer;
  int64_t total_durations; // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements; // The number of measurements made
};

// structure to capture statistics for deserialization latency
struct DeserializationLatency {
  // timer for the traditional bgp deserialization. May not hold end_time
  struct BenchmarkTimer bgp_deserialization_timer;
  int64_t total_durations; // the sum total of all durations measured (nanoseconds)
  int64_t num_measurements; // The number of measurements made
};
struct BgpBenchmarkStats {
  uint64_t advertisements_seen; // Number of advertisements seen during run
  struct timespec start_time; // The time when we started to receive advertisements
  struct timespec end_time; // The time when the benchmark ended
  // holds stats for the deserialization latency
  struct DeserializationLatency deserialization_latency;
  struct ProcessingLatency processing_latency;
  struct EndToEndLatency end_to_end_latency;
 };

typedef struct BgpBenchmarkStats* BgpBenchmarkStatsPtr;

int64_t GetNanoSecDuration(struct timespec start_time);

void PrintBenchmarkStats(struct BgpBenchmarkStats bgp_benchmark_stats);

#endif
