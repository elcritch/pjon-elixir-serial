use Mix.Config

config :pjon_elixir_serial, :device, System.get_env("MIX_UART") || "ttyACM0"

config :pjon_elixir_serial, :compile_options,
    packet_size: 128,
    include_packet_id: true,
    packet_max_length: 64,
    receive_while_sending_blocking: false,
    max_recent_ids: 4096,
    ts_response_timeout: 150000,
    pjon_send_blocking: true,
    rx_wait_time: 200000,
    ts_max_attempts: 20,
    bus_addr: 42,
    tx_packet_addr: 47

