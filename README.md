# Multitasking-OS

A C++ emulator of a multitasking operating system. It simulates multi-core CPU scheduling (FCFS and Round Robin), demand-paged memory management with FIFO page replacement, and a file-backed backing store, all driven from an interactive shell.

Built as the final major output for the CSOPESY course. The project specification is included as [`request.pdf`](request.pdf).

## Features

- **Interactive shell** with commands for starting and stopping the scheduler, creating processes, and inspecting CPU and memory state.
- **Multi-core scheduler** that runs one worker thread per emulated core. Supports First-Come-First-Served (`fcfs`) and Round Robin (`rr`) with a configurable quantum.
- **Demand paging** with a frame table, page faults, and FIFO page replacement.
- **Backing store** persisted to `csopesy-backing-store.txt`, readable at any time.
- **Process instruction set**: `PRINT`, `DECLARE`, `ADD`, `SUBTRACT`, `SLEEP`, `FOR`, `READ`, `WRITE`.
- **Automatic process generation** at a configurable frequency, each with a random instruction stream and memory size.
- **Monitoring tools**: `process-smi`, `vmstat`, `screen -ls`, and a `report-util` log file.

## Requirements

- A C++20 compiler: `clang++` or `g++` (the Unix script prefers `clang++`; the Windows script uses `g++` from MinGW).
- macOS and Windows build as-is. Linux needs a small fix, see [Building on Linux](#building-on-linux).

## Build and Run

Run everything from the repository root, because `config.txt` is read from the current working directory.

**macOS / Linux**

```bash
./compile.sh
./main
```

**Windows**

```bat
run.bat
main.exe
```

> `run.bat` is the Windows build script: it compiles the project into `main.exe`.

## Configuration

On `initialize`, the emulator reads `config.txt` (one `key value` pair per line):

| Key | Description | Default |
| --- | --- | --- |
| `num-cpu` | Number of emulated CPU cores | `1` |
| `scheduler` | Scheduling algorithm, lowercase: `fcfs` or `rr` | `rr` |
| `quantum-cycles` | Instructions per time slice (Round Robin only) | `10` |
| `batch-process-freq` | Generator ticks between automatically created processes | `60` |
| `min-ins` / `max-ins` | Range for the number of instructions in a generated process | `30` / `45` |
| `delay-per-exec` | Delay after each executed instruction (ms) | `2` |
| `max-overall-mem` | Total physical memory | `4096` |
| `mem-per-frame` | Size of one frame (and one page) | `64` |
| `min-mem-per-proc` / `max-mem-per-proc` | Allowed per-process memory range | `512` / `512` |

The number of frames is `max-overall-mem / mem-per-frame`. A process needs `ceil(memory / mem-per-frame)` pages.

## Shell Commands

Start with `initialize`; every other command (except `exit`) is rejected until the configuration is loaded.

| Command | Description |
| --- | --- |
| `initialize` | Load `config.txt` and set up the memory allocator |
| `scheduler-start` | Start the scheduler and begin generating processes automatically (`Main-Process<N>`) |
| `scheduler-stop` | Stop generating processes and signal the scheduler to stop |
| `screen -s <name> <memory>` | Create a named process that runs a standalone workload on its own thread |
| `screen -c <name> <memory> "<instructions>"` | Create a named process with a custom instruction list and hand it to the scheduler |
| `screen -r <name>` | Open a process's screen. Inside it, `process-smi` prints the process name, ID, and execution log; `exit` returns to the main console |
| `screen -ls` | Show CPU utilization, running processes, and finished processes |
| `process-smi` | Summary of memory usage and the processes currently resident in memory |
| `vmstat` | Memory totals, CPU ticks, page faults, and page swaps |
| `report-util` | Append a CPU utilization report to `csopesy-log.txt` |
| `exit` | Quit the emulator |

Start the scheduler with `scheduler-start` before using `screen -c`, `screen -ls`, `process-smi`, `vmstat`, or `report-util`.

`<memory>` must be a power of two between `min-mem-per-proc` and `max-mem-per-proc`. With the default config, 512 is the only valid value.

### Process instructions

Instructions passed to `screen -c` are quoted and separated by semicolons (1 to 50 instructions):

```
screen -c worker 512 "DECLARE x 5; ADD y x 10; PRINT hello; SLEEP 2"
```

| Instruction | Syntax | Behavior |
| --- | --- | --- |
| `PRINT` | `PRINT <message>` | Writes the message to the process's log |
| `DECLARE` | `DECLARE <var> <value>` | Declares a 16-bit variable; ignored if it already exists |
| `ADD` | `ADD <target> <op1> <op2>` | `target = op1 + op2`; operands are literals or variables (undeclared variables start at 0) |
| `SUBTRACT` | `SUBTRACT <target> <op1> <op2>` | `target = op1 - op2`, floored at 0 |
| `SLEEP` | `SLEEP <ticks>` | Pauses the process for `ticks` × 100 ms (but see [Known Limitations](#known-limitations) for how `screen -c` parses `<ticks>`) |
| `READ` | `READ <var> <address>` | Reads a 16-bit value from memory into a variable (see [Known Limitations](#known-limitations)) |
| `WRITE` | `WRITE <address> <var>` | Writes a 16-bit value to a memory address (see [Known Limitations](#known-limitations)) |
| `FOR` | generated only | Repeats a nested block of 2–4 instructions 1–5 times, up to 3 levels deep. Used by auto-generated processes |

## How It Works

### Scheduler

`Scheduler` starts one worker thread per core. Processes wait in a shared queue guarded by a mutex and condition variable.

- **FCFS** runs a process to completion once it is on a core.
- **Round Robin** runs up to `quantum-cycles` instructions, then returns the process to the back of the queue if it is unfinished.

A process only runs when all of its pages are resident. If memory cannot be assigned, the process is re-queued and an idle CPU tick is recorded.

### Memory management

`PagingAllocator` splits physical memory into fixed-size frames and loads a process's pages on demand. When no free frame is available, the oldest loaded page is evicted (FIFO) and written back to the backing store. Finished processes release their frames.

### Backing store

`csopesy-backing-store.txt` holds one line per page in the form `<pid> <page> <state>`. Lines are written when a process is created, and updated as pages are loaded (`-2`) and evicted (`-1`). The file is reset each time the emulator starts.

### Monitoring output

`vmstat` reports total, used, and free memory, idle/active/total CPU ticks, **Page Faults** (pages brought in), and **Page Swaps** (pages written out). `report-util` appends a timestamped utilization snapshot to `csopesy-log.txt`.

## Example Session

```text
root:\\ initialize
Configuration loaded successfully.
...
numFrames: 64
root:\\ scheduler-start
Scheduler started with 1 core(s)

root:\\ screen -c demo 512 "DECLARE x 5; ADD y x 10; PRINT hello; SLEEP 2"
Screen demo created successfully with custom instructions.
root:\\ screen -ls

--- Current Screen Status at T = (11/10/2026 18:48:07 PM) ---

CPU Utilization:  100%
Cores Used:       1
Cores Available:  0

Running Processes:
  demo   (11/10/2026 18:48:07 PM)   Core: 0   3/4

Finished Processes:
  Main-Process0   (11/10/2026 18:48:01 PM)   Finished   30/30
-------------------------------------------
root:\\ vmstat

--- VM Statistics at T = (11/10/2026 18:48:07 PM) ---

Total Memory: 4096 MiB
Used Memory: 512 MiB
Free Memory: 3584 MiB
Idle CPU Ticks: 0
Active CPU Ticks: 3
Total CPU Ticks: 3
Page Faults: 16
Page Swaps: 8
```

## Project Structure

```text
Multitasking-OS/
├── main.cpp                 # Entry point: resets log files, starts the main console
├── main-console.{h,cpp}     # Shell, config loading, scheduler control, reporting
├── screen-console.{h,cpp}   # Per-process screen and its log
├── scheduler.{h,cpp}        # Multi-core FCFS / Round Robin scheduler
├── process.{h,cpp}          # Process state, command list, symbol table
├── PagingAllocator.{h,cpp}  # Demand paging, frame table, FIFO replacement
├── IMemoryAllocator.h       # Allocator interface
├── config.{h,txt}           # Config struct and default configuration
├── vmstat.h, backingStore.h # Shared stats and backing-store lock
├── globals.{h,cpp}          # Shared global state
├── commands/                # One class per instruction (Print, Declare, Add, ...)
├── compile.sh, run.bat      # Build scripts (Unix / Windows)
└── *_test.cpp               # Standalone tests
```

`FlatMemoryAllocator.*` and `test.cpp` are from an earlier flat-allocation design and are not part of the build.

## Testing

Three standalone programs test the symbol table and the `READ`/`WRITE` commands. Build one, for example:

```bash
g++ -std=c++20 read_command_test.cpp PagingAllocator.cpp process.cpp globals.cpp screen-console.cpp commands/*.cpp -o read_command_test
./read_command_test
```

The same command works for `write_command_test.cpp` and `symbol_table_test.cpp`. On Linux, add the flags from [Building on Linux](#building-on-linux).

## Known Limitations

- `READ` and `WRITE` are implemented (`commands/ReadCommand.*`, `commands/WriteCommand.*`) and covered by their tests, but they are not yet wired into `screen -c` execution, and the build scripts do not compile them. `screen -c` accepts the syntax but does not run them.
- `SLEEP <ticks>` in `screen -c` input is parsed into a `uint8_t` with `>>`, which reads one *character* instead of a number. `SLEEP 2` therefore sleeps for 5 s (the character `'2'` is 50) and `SLEEP 10` for 4.9 s (`'1'` is 49). Auto-generated processes are unaffected. Parsing the value into an `int` first fixes this.
- `FOR` is not accepted in `screen -c` input; it appears only in auto-generated processes.
- `process-smi`, `screen -ls`, and `report-util` need the scheduler to be running.
- The specification defines memory in bytes, but `vmstat` and `process-smi` print sizes with a `MiB` label.
- Source files are written for macOS/Windows; see [Building on Linux](#building-on-linux).

## Authors

- Cabili, Johan Marlo
- Carandang, Matthew Ryan
- Veracruz, Sean Riley
