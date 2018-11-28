defmodule PjonElixirSerial.Router do
  require Logger
  use GenServer

  alias PjonElixirSerial.PjonRegistry


  def register(), do: register(self())

  def register(pid, type \\ :all) do
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

  def handle_cast({:route, {type, term} = _msg}) do
    # Dispatch
    Registry.dispatch(PjonRegistry, :listeners, fn entries ->
      for {_pid, item} <- entries,
      {:on, filter_type, client} = item,
      type == filter_type do
        send(client, {:data, term})
      end
    end)
  end
end
