# TI SimpleLink + Tuya IoT Integration

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![OS](https://img.shields.io/badge/OS-FreeRTOS-green.svg)
![Build](https://img.shields.io/badge/Build-Status%20Placeholder-lightgrey.svg)

**Work in progress:** the TuyaOS Library and Network Terminal integration is currently unfinished and remains under active development.

This repository contains the codebase for a BSc Computer Science final project at Ariel University, developed in collaboration with Texas Instruments (TI). The project integrates the Tuya IoT software stack into the TI SimpleLink ecosystem using C and FreeRTOS, with a focus on embedded connectivity, device provisioning, and application-level IoT workflows.

The overall goal is to evaluate how TuyaOS and TuyaLink can be adapted to a TI microcontroller platform while preserving the structure, portability, and development workflow expected from a production embedded SDK integration.

## Demo Video

[![Demo Video Placeholder](https://img.shields.io/badge/Demo-Video%20Placeholder-informational)](https://example.com/demo-video)

> Replace the link above with the project demo video URL when available.

## Project Overview

This project explores the integration of Tuya cloud connectivity features into the TI SimpleLink SDK environment. It combines embedded networking, RTOS-based tasking, and vendor-specific device abstractions to support three related implementation tracks:

- TuyaOS as a reusable library inside the TI SimpleLink environment
- TuyaLink integration over MQTT for network terminal-based demonstrations
- TuyaLink integration with CSI-based detection for a fully working end-to-end demo

From an embedded systems perspective, the work focuses on:

- adapting third-party SDK code to TI-specific toolchains and board support packages
- keeping the implementation compatible with FreeRTOS tasking and synchronization primitives
- managing Wi-Fi lifecycle, logging, networking, and provisioning flows in a constrained MCU environment
- preserving a clear separation between reusable adapter code and application-level logic

## Architecture / Project Breakdown

The repository is organized around three main components that will ultimately live under a shared GitHub Organization.

| Component | Description | Status |
|---|---|---|
| TuyaOS Library and Network Terminal (NT) Example | Integration of TuyaOS into the TI SimpleLink SDK as a library, with a Network Terminal example showing how the adapter layer is wired into the TI application flow. | In progress |
| TuyaLink via MQTT in NT Example | A working Network Terminal example that demonstrates TuyaLink communication using MQTT. | Working |
| TuyaLink with CSI Detection in NT Example | A fully functional collaboration that combines TuyaLink with Channel State Information (CSI) detection and includes a working demo. | Working |

### Component Notes

#### 1. TuyaOS Library and Network Terminal Example

This branch of the project focuses on making TuyaOS available as a library inside the TI SimpleLink build environment. It includes adapter code for Wi-Fi, networking, logging, semaphores, system services, and other hardware-facing interfaces required by the Tuya stack.

This component is explicitly unfinished and may continue to change as the adapter layer is refined.

#### 2. TuyaLink via MQTT in NT Example

This example demonstrates a functional TuyaLink path over MQTT within the Network Terminal application. It serves as a reference implementation for cloud communication, provisioning flow integration, and runtime networking behavior.

#### 3. TuyaLink with CSI Detection in NT Example

This example combines TuyaLink with CSI detection for a full working demonstration. It is the most complete integration path in the project and is intended to showcase both cloud connectivity and sensing-oriented functionality.

## Getting Started

### Prerequisites

- TI SimpleLink SDK for the target CC35xx platform
- A supported TI toolchain and CMake-based build environment
- FreeRTOS and the board configuration used by the selected TI example
- Tuya SDK sources and any required precompiled Tuya libraries

### Build

```bash
cmake -S . -B build/ticlang -G Ninja -DTICLANG_ARMCOMPILER="C:/ti/ccs${CCS_VERSION}/ccs/tools/compiler/ti-cgt-armllvm_${COMPILER_VERSION}.LTS"
cmake --build build/ticlang
```

> The exact generator, toolchain file, and target name depend on the selected TI workspace configuration.
> This repository expects the build directory to match the toolchain suffix, such as `build/ticlang`.

### Flashing to the TI MCU

1. Build the desired target for the selected board.
2. Flash the generated image using the TI development tools or the configured flashing workflow.
3. Open the serial console and verify the boot, network initialization, and Tuya runtime logs.

## Team

| Name | LinkedIn |
|---|---|
| Gal Maymon | [Profile](https://www.linkedin.com/in/gal-maymon-a3881a244/) |
| Or Bibi | [Profile](https://www.linkedin.com/in/or-bibi/) |
| Samuel Lazareanu | [Profile](https://www.linkedin.com/in/samuellazareanu/) |
| Amit Nachum | [Profile](https://www.linkedin.com/in/amit-nachum-003992354/) |

## Acknowledgments

- TI Engineers: Dan Horowitz and Israel Zilbershmidet
- University Professor: Prof. Amit Dvir

## Disclaimer

This is an academic final project created by students. It is not an official Texas Instruments product, and the contributors do not represent Texas Instruments.
