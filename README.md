# Electronics and IoT

A collection of electronics and IoT projects spanning three simulation platforms: [Proteus](https://www.labcenter.com/), [Tinkercad](https://www.tinkercad.com/), and [Wokwi](https://wokwi.com/). The repository combines pure electronic digital circuits with IoT systems built around Node-RED and Blynk.

## Structure

### `proteus/`

Projects designed in Proteus Design Suite (`.pdsprj`).

- `secure_vending_machine`
- `simple_computer`

### `tinkercad/`

Pure electronic digital circuits simulated in Tinkercad. Each subfolder contains the circuit file (`.brd`) and a link to the live Tinkercad project.

- `1_to_4_demux` — 1-to-4 demultiplexer
- `16_bit_shift_register` — 16-bit shift register
- `2_bit_synchronous_up_counter` — 2-bit synchronous up counter
- `2_to_4_decoder` — 2-to-4 decoder
- `4_bit_adder` — 4-bit adder
- `4_bit_asynchronous_down_counter` — 4-bit asynchronous down counter
- `4_to_1_mux` — 4-to-1 multiplexer
- `4_to_2_encoder` — 4-to-2 encoder
- `555_timer` — 555 timer
- `decade_counter` — decade counter
- `full_adder` — full adder
- `full_subtractor` — full subtractor
- `jk_flip_flop_counter` — JK flip-flop counter

### `wokwi/`

IoT projects simulated in Wokwi using ESP32 boards. These projects connect to the cloud via [Blynk](https://blynk.io/) and MQTT (consumed by [Node-RED](https://nodered.org/)) for monitoring and control.

- `env_sense_controller` — environment sensing controller (Blynk)
- `multi_sensor_monitoring_system` — multi-sensor monitoring system
- `smart_door_system` — smart door system (Blynk)
- `smart_home_system` — smart home system (MQTT / Node-RED)
