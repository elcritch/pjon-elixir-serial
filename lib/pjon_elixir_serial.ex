defmodule PjonElixirSerial do
  use Application

  @moduledoc """
  Documentation for PjonElixir.
  """

  def start(_type, _args) do
    children = [
      {Registry, keys: :duplicate, name: PjonElixirSerial.PjonRegistry},
      {PjonElixirSerial.Proc, name: PjonElixirSerial.Proc}
    ]

    Supervisor.start_link(children, strategy: :one_for_one)
  end
end
