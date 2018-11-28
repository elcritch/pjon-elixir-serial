
defmodule PjonElixirSerial.Parser do
  require Logger
  use Task

  @seperator Application.get_env(
    :pjon_elixir_serial,
    :parser_separator,
    ["\n\6", "\n"]
  )

  @timeout Application.get_env(:pjon_elixir_serial, :parser_timeout, 100)
  @max_buffer Application.get_env(:pjon_elixir_serial, :parser_max_buffer, 8192)

  # def start_link(args \\ []) do
  #   Task.start_link(__MODULE__, :stream_parser, [])
  # end

  def stream_parser() do
    Logger.error("starting parser: #{inspect self()}")
    Process.register(self(), PjonElixirSerial.Parser)
    Stream.repeatedly(&receive_data_packet/0)
    |> Stream.transform("", &framer_func/2)
    |> Stream.map(&framer_func/2)
    |> Stream.run()
  end

  def frame(line, kind \\ :data) do
    {kind, line}
  end

  def framer_func(msg, acc) do 
    case msg do
      {:packet, data} ->
        items = String.split(acc <> data, @seperator, trim: false);
        {last_item, lines} = List.pop_at(items, -1, "");

        frames = lines |> Enum.map(&frame/1)
        if byte_size(last_item) < @max_buffer do
          {[frames], last_item} # the last line is always the last item
        else
          {[frames, [last_item |> frame(:partial) ]], ""} # overflow, push out packet
        end
      :timeout ->
        {acc, ""}
    end
  end

  def dispatch_type({_type, _term} = msg) do
    GenServer.cast(PjonElixirSerial.Router, {:route, msg})
  end

  def receive_data_packet() do
    receive do
      {:packet, bindata} = msg ->
        Logger.error("port data: command: #{inspect(msg)}")
        {:packet, bindata}
    after
      @timeout ->
        :timeout
    end
  end

end
