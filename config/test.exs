use Mix.Config

config :pjon_elixir_serial, :device, System.get_env("MIX_UART") || "ttyACM0"

# device_type = System.get_env("PJON_DEVICE_TYPE") || "LINUX"

config :pjon_elixir_serial, :compile_options,
    device_type: "RPI",
    debug_verbose: true,
    packet_size: 128,
    include_packet_id: true,
    max_recent_ids: 4096,
    ts_response_timeout: 150000,
    pjon_send_blocking: true,
    rx_wait_time: 2000,
    ts_max_attempts: 20,
    bus_addr: 42,
    tx_packet_addr: 47


# IO.inspect device_type, label: :DEVICE

