defmodule PjonElixirSerial do
  require Logger
  use Application

  @moduledoc """
  Documentation for PjonElixir.
  """

  def start(_type, _args) do
    children = [
      {Registry, keys: :duplicate, name: PjonElixirSerial.PjonRegistry},
      {PjonElixirSerial.DeviceManager, name: PjonElixirSerial.DeviceManager},
      {PjonElixirSerial.Port, name: PjonElixirSerial.Port},
    ]

    Supervisor.start_link(children, strategy: :rest_for_one)
  end

  def whereis(name) when is_binary(name) do
    # TODO: support multiple instances... 
    Process.whereis(PjonElixirSerial.DeviceManager)
  end

  def write(pid, data) do
    GenServer.call(pid, {:command, data})
  end
end
