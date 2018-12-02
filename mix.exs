defmodule PjonElixirSerial.MixProject do
  use Mix.Project

  def pjon_option(option, default) do
    Application.get_env(:pjon_elixir_serial, :compile_options, [])
    |> Keyword.get(option, default)
  end

  def project do
    [
      app: :pjon_elixir_serial,
      version: "0.1.3",
      elixir: "~> 1.5",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_env: %{
        # "" => "#{pjon_option(:, 0)}",
        "PJON_RECEIVE_WHILE_SENDING_BLOCKING" => "#{pjon_option(:receive_while_sending_blocking, false)}",
        "SERIAL_FREAD_LOOP_DELAY" => "#{pjon_option(:serial_fread_loop_delay, 20000)}",
        "SERIAL_SREAD_LOOP_DELAY" => "#{pjon_option(:serial_sread_loop_delay, 20000)}",
        "PJON_STRATEGY" => "#{pjon_option(:pjon_strategy, "ThroughSerial")}",
        "PJON_PACKET_MAX_LENGTH" => "#{pjon_option(:packet_max_length, 64)}",
        "PJON_INCLUDE_PACKET_ID" => "#{pjon_option(:include_packet_id, true)}",
        "PJON_MAX_RECENT_PACKET_IDS" => "#{pjon_option(:max_recent_ids, 10)}",
        "TS_RESPONSE_TIME_OUT" => "#{pjon_option(:ts_response_timeout, 15100)}",
        "BUS_ADDR" => "#{pjon_option(:bus_addr, 42)}",
        "TX_PACKET_ADDR" => "#{pjon_option(:tx_packet_addr, 47)}",
        "PJON_SEND_BLOCKING" => "#{pjon_option(:blocking_send, true)}",
        "PJON_RX_WAIT_TIME" => "#{pjon_option(:rx_wait_time, 100)}",
        "TS_MAX_ATTEMPTS" => "#{pjon_option(:ts_max_attempts, 10)}",
        "DEBUG_MODE" => "#{pjon_option(:debug_mode, false)}",
        "DEBUG_LOGFILE" => "#{pjon_option(:debug_mode_logfile, "/tmp/pjon_serial.txt")}",
      },
      deps: deps()
    ]
  end

  def application do
    [
      mod: {PjonElixirSerial, []},
      extra_applications: [:logger]
    ]
  end

  defp package do
    [
      # ...
      files: [
        # These are the default files
        "lib",
        "LICENSE",
        "mix.exs",
        "README.md",
        "src/",
        # You will need to add something like this.
        "Makefile"
      ]
      # ...
    ]
  end

  defp deps do
    [
      {:elixir_make, "~> 0.4", runtime: false},
      {:msgpax, "~> 2.0"}
    ]
  end
end
