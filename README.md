# Kaurea

**Version:** 1.1.0
**Status:** Active

---

## Description

Kaurea is a hashing method based on heavy mathematical transformations and non-linear functions, producing hashes with a fixed size of 1024 bits. It is designed for high-entropy outputs and extended lifespan in cryptographic contexts.

---

## Purpose and Scope

**Problem**
Provide larger and more complex hashing mechanisms aimed at increasing security margins and long-term resilience.

**Target Users**

* Enthusiasts
* Developers
* Companies evaluating technical capability

**Use Cases**

* Application development
* Protocol design
* User data protection

---

## Features

**Core Features**

* Non-linear transformations:

  * Maps
  * Bitwise and bytewise operations
  * Shuffling and permutations
  * Fusions and ARX operations
* Fixed 1024-bit hash output
* Built-in test vector system
* Output format: `char*`

**Non-Goals**

* Any illegal usage
* Any usage unrelated to hashing systems

---

## Tech Stack

**Language**

* C (C99 standard)

**Libraries**

* None (pure/vanilla C implementation)

---

## Installation

**Prerequisites**

* C99-compatible compiler
* OS: Linux or macOS

---

## Quick Start

```bash
# Hash execution
chmod +x scripts/activate.sh
./scripts/activate.sh

# Test vectors
chmod +x scripts/activate_testvector.sh
./scripts/activate_testvector.sh
```

---

## Usage

**Input**

* Plain text (`char*`)

**Output**

* 1024-bit hash represented as a string (`char*`)

---

## Architecture Overview

```text id="1g8skw"
Input
  ↓
Salting
  ↓
Compression Phase (cof)
  - pass/or
  - fill/expansion
  ↓
16x Processing Loop
  - Disassemble
  - Apply mathematical transformations
  - Reassemble
  - Shuffle
  ↓
Hexbox Conversion
(uint8_t* → char*)
```

---

## Configuration

Defined as constants in the implementation:

* `const input`
* `const input_length`
* `const salt_rounds`
* `const hash_length`

---

## Testing

**Execution**

* Run provided scripts in `scripts/`

**Purpose**

* Validate deterministic outputs
* Verify internal consistency

---

## Build and Integration

* Follow installation steps
* Integrate by including source files from `src/`
* Do not include:

  * `main.c`
  * `testvector.c`

**Notes**

* For other languages, adaptation is required:

  * FFI (Foreign Function Interface)
  * Manual function adaptation

---

## Limitations

* Constrained by system architecture:

  * 32-bit systems: up to 4,294,967,295 bytes input
  * 64-bit systems: up to 18,446,744,073,709,551,615 bytes input

---

## Security

* Experimental hashing method
* No formal cryptographic audit
* No guarantee against reverse engineering or collision resistance
* Use at your own risk

---

## Contributing

* Contributions are not accepted
* You may create and distribute your own version under these conditions:

  * Clearly state it is a modified copy
  * Do not present it as the original implementation

---

## Disclaimer

This project does guarantee **theorical** cryptographic security, BUT there isnt profisional level of testing, only local and theorical results, that guarantee theorical security.

---

## License

MIT

---

## Maintainers

* THI100 (solo project)
