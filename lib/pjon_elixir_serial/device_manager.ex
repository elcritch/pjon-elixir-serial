defmodule PjonElixirSerial.DeviceManager do
  require Logger
  use GenServer

  alias PjonElixirSerial.PjonRegistry


  def register(), do: register(self())

  def register(pid, type \\ :any) do
   {:ok, _} = Registry.register(PjonRegistry, :listeners, {:on, type, pid})
  end

  @doc """
  """
  def start_link(opts \\ []) do
    GenServer.start_link(__MODULE__, opts, name: __MODULE__)
  end

  def init(_opts) do
    Task.start_link(PjonElixirSerial.Parser, :stream_parser, [])
    {:ok, %{}}
  end

  def handle_info({:command, _data} = msg , %{} = state) do
    GenServer.cast(PjonElixirSerial.Port, msg)
    {:noreply, state}
  end

  def handle_cast({:command, _data} = msg , %{} = state) do
    GenServer.cast(PjonElixirSerial.Port, msg)
    {:noreply, state}
  end

  def handle_call({:command, _data} = msg , %{} = state) do
    # TODO: change this to a blocking call, but need application specific "READY" code
    GenServer.cast(PjonElixirSerial.Port, msg)
    {:noreply, state}
  end

  def handle_cast({:route, {type, term} = msg}, state) do
    # Dispatch
    Logger.debug("router: dispatch: #{inspect(msg)}")
    Registry.dispatch(PjonRegistry, :listeners, fn entries ->
      Logger.debug("router: dispatch entries: #{inspect(entries)}")
      for {_pid, item} <- entries,
          {:on, filter_type, client} = item,
          type == filter_type || filter_type == :any do
        Logger.debug("router: dispatch to: #{inspect(client)}")
        send(client, {:pjon, {:data, term}})
      end
    end)
    {:noreply, state}
  end
end
