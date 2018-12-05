use Mix.Config

baud_rate = System.get_env("UART_BAUD") || to_string(921600)
device = System.get_env("UART_DEVICE") || "ttyACM0"

config :pjon_elixir_serial, :baud_rate, baud_rate
config :pjon_elixir_serial, :device, device

device_type = (System.get_env("PJON_DEVICE_TYPE") || "LINUX")

config :pjon_elixir_serial, :compile_options,
  device_type: device_type,
  send_type: "send",
  serial_fread_loop_delay: 50_000,
  serial_sread_loop_delay: 50_000,
  packet_size: 128,
  include_packet_id: true,
  max_recent_ids: 4096,
  max_packets: 20,
  packet_max_length: 128,
  ts_response_timeout: 34_000,
  blocking_send: false,
  blocking_verbose: 0,
  back_off_degree: 6,
  receive_while_sending_blocking: false,
  rx_wait_time: 100_00,
  ts_max_attempts: 10,
  bus_addr: 42,
  tx_packet_addr: 47


# IO.inspect device_type, label: :DEVICE


  # bus.send_packet_blocking
