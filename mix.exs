defmodule PjonElixirSerial.MixProject do
  use Mix.Project

  def project do
    [
      app: :pjon_elixir_serial,
      version: "0.1.2",
      elixir: "~> 1.5",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_env: %{
        "PACKET_SIZE" => "#{Application.get_env(:pjon_elixir_serial, :packet_size, 256)}",
        "PJON_INCLUDE_PACKET_ID" => "#{Application.get_env(:pjon_elixir_serial, :include_packet_id, true)}",
        "PJON_MAX_RECENT_PACKET_IDS" => "#{Application.get_env(:pjon_elixir_serial, :max_recent_ids, 10)}",
        "TS_RESPONSE_TIME_OUT" => "#{Application.get_env(:pjon_elixir_serial, :response_timeout, 1010)}",
        "BUS_ADDR" => "#{Application.get_env(:pjon_elixir_serial, :bus_addr, 42)}",
        "TX_PACKET_ADDR" => "#{Application.get_env(:pjon_elixir_serial, :TX_PACKET_ADDR, 47)}",
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
