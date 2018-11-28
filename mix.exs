defmodule PjonElixirSerial.MixProject do
  use Mix.Project

  def project do
    [
      app: :pjon_elixir_serial,
      version: "0.1.2",
      elixir: "~> 1.5",
      start_permanent: Mix.env() == :prod,
      compilers: [:elixir_make] ++ Mix.compilers(),
      deps: deps()
    ]
  end

  def application do
    [
      mod: {PjonElixirSerial, []},
      extra_applications: [:logger]
    ]
  end

  defp package do
    [
      # ...
      files: [
        # These are the default files
        "lib",
        "LICENSE",
        "mix.exs",
        "README.md",
        "src/",
        # You will need to add something like this.
        "Makefile"
      ]
      # ...
    ]
  end

  defp deps do
    [
      {:elixir_make, "~> 0.4", runtime: false},
      {:msgpax, "~> 2.0"}
    ]
  end
end
