defmodule PjonElixirSerial.Port do
  require Logger

  @moduledoc """
  Documentation for PjonElixirSerial.
  """

  use GenServer

  def pack!(data) do
    # MsgPax.pack!(data)
    data
  end

  def unpack!(data) do
    # MsgPax.unpack!(data)
    data
  end

  @doc """
  """
  def start_link(opts \\ []) do
    Logger.info("Starting Pjon PORT")
    GenServer.start_link(__MODULE__, opts, name: __MODULE__)
  end

  @init_arg Application.get_env(:pjon_elixir_serial, :init_arg, "\n")

  def init(opts) do
    Logger.info("Starting Pjon PORT")
    baud_rate = Application.get_env(:pjon_elixir_serial, :baud_rate, 115_200)
    device_name = Application.get_env(:pjon_elixir_serial, :device, "/dev/ttyUSB0")

    port_args = ["#{device_name}", "#{baud_rate}"]

    port_lib = :code.priv_dir(:pjon_elixir_serial)
    port_bin = Path.join(port_lib, "pjon_serial")

    port_opts = [{:args, port_args}, :binary,
                 :exit_status,
                 packet: 2,
                 env: [{'LD_LIBRARY_PATH': port_lib}]
                ]

    GenServer.cast(self(), :start)
    {:ok, %{port: nil, opts: port_opts, bin: port_bin}}
  end

  def handle_cast(:start, %{opts: port_opts, bin: port_bin} = state) do
    # Start Port Binary
    Logger.info("Opening uart with options: #{inspect(port_opts)} -- bin: #{inspect(port_bin)}")
    port = Port.open({:spawn_executable, "#{port_bin}"}, port_opts)

    no_init? = Application.get_env(:pjon_elixir_serial, :no_init, false)

    unless no_init? do
      send(port, {self(), {:command, pack!(@init_arg)}})
    end

    {:noreply, %{ state | port: port }}
  end

  def handle_info({_port, {:exit_status, 0}}, %{} = state) do
    Logger.debug("PJON Serial port: closing: #{inspect(self())}")
    {:stop, :normal, state}
  end

  def handle_info({_port, {:exit_status, _} = error}, %{} = state) do
    Logger.debug("PJON Serial port: closing: error: #{inspect(self())} -- #{inspect error}")
    {:stop, :port_terminated, state}
  end

  def handle_info({_port, {:data, rawdata} = msg}, state) do
    term = rawdata |> unpack!()

    parser = Process.whereis(PjonElixirSerial.Parser)
    Logger.debug("port sent packet data: #{inspect(msg)}")
    send(parser, {:packet, term})
    {:noreply, state}
  end

  def handle_info({:data, rawdata} = msg, state) do
    term = rawdata |> unpack!()

    parser = Process.whereis(PjonElixirSerial.Parser)
    Logger.debug(" sent packet data: #{inspect(msg)}")
    send(parser, {:packet, term})
    {:noreply, state}
  end

  def handle_cast({:command, term}, %{port: port} = state) do
    Logger.debug("port data: command cast: #{inspect(term)}")
    send(port, {self(), {:command, term |> pack!}})

    {:noreply, state}
  end
end
