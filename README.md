## loon – High-Performance, Header-Only C++ Library

**loon** is a lightweight, header-only C++ library designed for low memory footprint, low latency, and maximum performance. It provides optimized data structures and utilities that outperform standard STL counterparts while maintaining full STL compatibility.

---

### Key Features:
- Zero-Cost Abstractions: Header-only design with no external dependencies. 
- STL-Compliant: Drop-in replacements with familiar interfaces. 
- Cache-Efficient: Optimized memory layout (SoA, pooling, alignment). 
- Low Latency: Avoids dynamic allocation on critical paths; ideal for real-time systems. 
- High Performance: Benchmarked against std:: types—faster insertion, lookup, and iteration.

Perfect for performance-critical applications in HFT, gaming, embedded systems, and real-time processing.