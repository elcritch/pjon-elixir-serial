defmodule PjonElixirSerial.Application do
  require Logger
  use Application

  @moduledoc """
  Documentation for PjonElixir.
  """

  def start(_type, _args) do
    children = [
      {Registry, keys: :duplicate, name: PjonElixirSerial.PjonRegistry},
      {PjonElixirSerial.Router, name: PjonElixirSerial.Router},
      {PjonElixirSerial.Port, name: PjonElixirSerial.Port},
    ]

    Supervisor.start_link(children, strategy: :rest_for_one)
  end
end
