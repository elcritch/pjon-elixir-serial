defmodule PjonElixir.Proc do
  @moduledoc """
  Documentation for PjonElixir.
  """

  use GenServer

  def to_binary(data) do
    MsgPax.binary_to_term(data)
  end

  @doc """
  """
  def start_link(opts \\ []) do
    GenServer.start_link(PjonElixir.Proc, opts, name: __MODULE__)
  end

  def init(opts) do
    port = Port.open({:spawn, '#{cmd}'}, [:binary, :exit_status, packet: 2] ++ opts)

    if Keyword.get(opts, :no_init, false) do
      send(port, {self(), {:command, :erlang.term_to_binary(initarg)}})
    end

    {:ok, %{port: port}}
  end

  def handle_info({port, {:exit_status, 0}}, %{port: port} = state) do
    {:stop, :normal, state}
  end

  def handle_info({port, {:exit_status, _}}, %{port: port} = state) do
      {:stop, :port_terminated, state}
  end

  def handle_info({port, {:data, data}}, %{port: port} = state) do
    if event_manager do
      :gen_event.notify(event_manager, )
    end
    {:noreply, state}
  end

  def handle_cast(term, {port, _} = state) do
    send(port, {self(), {:command, :erlang.term_to_binary(term)}})
    {:noreply, state}
  end

  def handle_call(term, _reply_to, {port, _} = state) do
    send(port, {self(), {:command, :erlang.term_to_binary(term)}})

    res =
      receive do
        {^port, {:data, b}} ->
          :erlang.binary_to_term(b)

        # catch exit msg and resend it
        {^port, {:exit_status, _}} = exit_msg ->
          send(self(), exit_msg)
          {:error, :port_terminated}
      end

    {:reply, res, state}
  end
end

