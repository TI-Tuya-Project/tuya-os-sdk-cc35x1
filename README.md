# TI SimpleLink + TuyaOS Integration

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![OS](https://img.shields.io/badge/OS-FreeRTOS-green.svg)
![Build](https://img.shields.io/badge/Build-Status%20Placeholder-lightgrey.svg)

**Work in progress:** the TuyaOS Library and Network Terminal integration is currently unfinished and remains under active development.

This repository contains the codebase for a BSc Computer Science final project at Ariel University, developed in collaboration with Texas Instruments (TI). The project integrates TuyaOS into the TI SimpleLink / FreeRTOS environment using C, with a focus on adapter-layer integration for embedded networking and device services.

The overall goal is to evaluate how TuyaOS can be adapted to a TI microcontroller platform while preserving the structure, portability, and development workflow expected from a production embedded SDK integration.

## Project Overview

This project explores the integration of the TuyaOS software stack into the TI SimpleLink SDK environment. It combines embedded networking, RTOS-based tasking, and vendor-specific device abstractions to support a TuyaOS adapter/library integration for the TI platform.

From an embedded systems perspective, the work focuses on:

- adapting third-party SDK code to TI-specific toolchains and board support packages
- keeping the implementation compatible with FreeRTOS tasking and synchronization primitives
- managing Wi-Fi lifecycle, logging, networking, and provisioning flows in a constrained MCU environment
- preserving a clear separation between reusable TuyaOS adapter code and application-level logic

## TuyaOS References

- TuyaOS documentation and ecosystem: https://developer.tuya.com/en/docs/iot
- Tuya GitHub organization: https://github.com/tuya
- TuyaOS SDK reference entry point: https://developer.tuya.com/en/docs/iot/tuyaos-overview

## Disclaimer

This is an academic final project created by students. It is not an official Texas Instruments product, and the contributors do not represent Texas Instruments.
