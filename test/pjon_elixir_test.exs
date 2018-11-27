defmodule PjonElixirTest do
  use ExUnit.Case
  doctest PjonElixirSerial

  test "greets the world" do
    assert PjonElixir.hello() == :world
  end
end
