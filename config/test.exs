use Mix.Config

config :pjon_elixir_serial, :device, System.get_env("MIX_UART") || "ttyACM0"

device_type = (System.get_env("PJON_DEVICE_TYPE") || "LINUX")

config :pjon_elixir_serial, :compile_options,
  device_type: device_type,
  send_type: "sends",
  serial_fread_loop_delay: 30_000,
  serial_sread_loop_delay: 40_000,
  packet_size: 128,
  include_packet_id: true,
  max_recent_ids: 4096,
  max_packets: 100,
  packet_max_length: 128,
  ts_response_timeout: 154_000,
  blocking_send: false,
  blocking_verbose: 0,
  back_off_degree: 6,
  receive_while_sending_blocking: false,
  rx_wait_time: 100_000,
  ts_max_attempts: 10,
  bus_addr: 42,
  tx_packet_addr: 47


# IO.inspect device_type, label: :DEVICE


  # bus.send_packet_blocking
