find_package(benchmark REQUIRED)

add_executable(loon_benchmarks
    bench_ring_buffer.cpp
    bench_spsc.cpp
)

target_link_libraries(loon_benchmarks
PRIVATE
    loon
    benchmark::benchmark
    benchmark::benchmark_main
)

# Optional: Register benchmarks with CTest (runs with default iterations)
add_test(NAME benchmarks COMMAND loon_benchmarks --benchmark_min_time=0.1s)
