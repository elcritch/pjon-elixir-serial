# PjonElixirSerial

Wrapper around the PJON library configured for interfacing with UART (Serial) ports. It's currently functional (tested on Linux and macOS) but rough around the edges. Currently no support is given for checking packet statuses, changing settings at runtime, or using multiple devices. 

Currently only the `ThroughSerial` strategy is supported, though a future "compile time" option could be added. 

## Usage

Setup `config.exs`:

```elixir
config :pjon_elixir_serial, :compile_options,
    packet_size: 128,
    include_packet_id: true,
    max_recent_ids: 255,
    response_timeout: 12101,
    bus_addr: 42,
    tx_packet_addr: 47,
    rx_wait_time: 100,
    ts_max_attempts: 20

config :pjon_elixir_serial, :parser_timeout, 1_000
config :pjon_elixir_serial, :parser_max_buffer, 8192
config :pjon_elixir_serial, :parser_separator, [<<"\n", 0x6>>, <<0x6>>, "\n"]
config :pjon_elixir_serial, :init_arg, "\n"
config :pjon_elixir_serial, :baud_rate, 115_200
config :pjon_elixir_serial, :device, "/dev/ttyUSB0"
```

See [PJON](https://github.com/gioblu/PJON) for details regarding the compile time options. 

Example for writing data (note that `whereis` device handle currently does nothing): 

```elixir
  device = PjonElixirSerial.whereis("ttyUSB0")
  :ok = PjonElixirSerial.write(device, "hello world\n")
  :ok = PjonElixirSerial.write(device, %{key: 1} |> MsgPax.pack!)
```

Example for receiving data:

```elixir
    PjonElixirSerial.DeviceManager.register(self())
    
    receive do
       {:pjon, {:data, strmsg}} ->
          IO.puts "Got Message: #{strmsg}"
       {:pjon, {:partial, strmsg}} ->
          IO.puts "Got Partial: #{strmsg}"
    end
```

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `pjon_elixir` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:pjon_elixir_serial, "~> 0.1.2"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/pjon_elixir](https://hexdocs.pm/pjon_elixir).

