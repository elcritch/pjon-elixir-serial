defmodule PjonElixir do
  use Application

  @moduledoc """
  Documentation for PjonElixir.
  """

  def start(_type, _args) do
    children = [
      {PjonElixir.Proc, name: PjonElixir.Proc}
    ]
    Supervisor.start_link(children, strategy: :one_for_one)
  end

end
