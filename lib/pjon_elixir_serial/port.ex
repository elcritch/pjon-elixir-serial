defmodule PjonElixirSerial.Proc do
  require Logger

  @moduledoc """
  Documentation for PjonElixirSerial.
  """
  alias PjonElixirSerial.PjonRegistry

  use GenServer

  def pack!(data) do
    data
    # MsgPax.pack!(data)
  end

  def unpack!(data) do
    data
    # MsgPax.unpack!(data)
  end

  @doc """
  """
  def start_link(opts \\ []) do
    GenServer.start_link(__MODULE__, opts, name: __MODULE__)
  end

  @init_arg Application.get_env(:pjon_elixir_serial, :init_arg, "\n")

  def init(opts) do
    baud_rate = Application.get_env(:pjon_elixir_serial, :baud_rate, 115200)
    device_name = Application.get_env(:pjon_elixir_serial, :device, "/dev/ttyUSB0")

    port_args = [:args, ["#{device_name}", "#{baud_rate}"]]

    port_wrapper = Path.join(:code.priv_dir(:pjon_elixir_serial), "wrapper.sh")
    port_bin = Path.join(:code.priv_dir(:pjon_elixir_serial), "pjon_serial")
    Logger.debug("Opening uart with binary: #{inspect port_bin}")

    port_opts = [{:args, ["#{device_name}", "#{baud_rate}"]},
                 :binary,
                 :exit_status,
                 packet: 2]

    Logger.info("Opening uart with options: #{inspect port_opts}")

    # Start Port Binary
    port = Port.open({:spawn_executable,
                      "#{port_bin}"}, port_opts)

    unless Keyword.get(opts, :no_init, false) do
      send(port, {self(), {:command, pack!(@init_arg)}})
    end

    {:ok, %{port: port}}
  end

  def handle_info({port, {:exit_status, 0}}, %{port: port} = state) do
    {:stop, :normal, state}
  end

  def handle_info({port, {:exit_status, _}}, %{port: port} = state) do
    {:stop, :port_terminated, state}
  end

  def handle_info({port, {:data, rawdata} = msg}, %{port: port} = state) do
    data = rawdata |> unpack!()

    Logger.error("port rx data: #{inspect msg}")
    # Dispatch
    Registry.dispatch(PjonRegistry, :listeners, fn entries ->
      for {_pid, item} <- entries,
          {:on, :data, topid} = item do
        send(topid, {:serial, :data, data})
      end
    end)

    {:noreply, state}
  end

  def register(), do: register(self())

  def register(pid) do
    {:ok, _} = Registry.register(PjonRegistry, :listeners, {:on, :data, pid})
  end

  def handle_cast(term, %{port: port} = state) do
    Logger.error("port data: command: #{inspect term}")
    send(port, {self(), {:command, term |> pack!}})

    {:noreply, state}
  end

  def handle_call(term, _reply_to, %{port: port} = state) do
    send(port, {self(), {:command, term |> pack!}})

    res =
      receive do
        {^port, {:data, rawdata} = msg} ->
          Logger.error("port data: command: #{inspect msg}")
          unpack!(rawdata)

        # catch exit msg and resend it
        {^port, {:exit_status, _}} = exit_msg ->
          self()
          |> send(exit_msg)

          {:error, :port_terminated}
      end

    {:reply, res, state}
  end
end
