defmodule PjonElixirSerial.Proc do
  @moduledoc """
  Documentation for PjonElixirSerial.
  """

  use GenServer

  def pack!(data) do
    MsgPax.pack!(data)
  end

  def unpack!(data) do
    MsgPax.unpack!(data)
  end

  @doc """
  """
  def start_link(opts \\ []) do
    GenServer.start_link(PjonElixir.Proc, opts, name: __MODULE__)
  end

  @init_arg Application.get_env(:pjon_elixir_serial, init_arg, "\n")

  def init(opts) do
    port_opts = [:binary, :exit_status, packet: 2]
    port_bin = Path.join(:code.priv_dir(:my_app), "pjon_serial")

    # Start Port Binary
    port = Port.open({:spawn, '#{port_bin}'}, port_opts ++ opts)

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

  def register(), do: register(self())

  def register(pid) do
    {:ok, _} = Registry.register(PjonElixirSerial.Registry, :listeners, :data)
  end

  def handle_info({port, {:data, rawdata}}, %{port: port} = state) do
    data = upack!(rawdata)

    Registry.dispatch(PjonElixirSerial.Registry, :listeners, fn entries ->
      for {pid, :data} <- entries do
        send(pid, {:serial, :data, data})
      end
    end)

    {:noreply, state}
  end

  def handle_cast(term, {port, _} = state) do
    send(port, {self(), {:command, pack(term)}})
    {:noreply, state}
  end

  def handle_call(term, _reply_to, {port, _} = state) do
    send(port, {self(), {:command, pack(term)}})

    res =
      receive do
        {^port, {:data, rawdata}} ->
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
