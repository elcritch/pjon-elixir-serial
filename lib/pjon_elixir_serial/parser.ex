
defmodule PjonElixirSerial.Parser do
  require Logger
  use Task

  @seperator Application.get_env(
    :pjon_elixir_serial,
    :parser_separator,
    [<<"\n", 0x6>>, <<0x6>>, "\n"]
  )

  @timeout Application.get_env(:pjon_elixir_serial, :parser_timeout, 1_000)
  @max_buffer Application.get_env(:pjon_elixir_serial, :parser_max_buffer, 8192)

  def stream_parser(args \\ []) do
    Logger.error("starting parser: #{inspect self()}")
    Process.register(self(), PjonElixirSerial.Parser)

    Stream.repeatedly(&receive_data_packet/0)
    |> Stream.transform("\n", &framer_func/2)
    |> Stream.flat_map(&(&1))
    |> Stream.each(fn x -> Logger.error("parser result: #{inspect x}") end)
    |> Stream.each(&dispatch_type/1)
    |> Stream.run()
  end

  def frame(line, kind \\ :data) do
    {kind, line}
  end

  def framer_func(msg, acc) do 
    # Logger.error("framer_func got: #{inspect msg}, acc: #{inspect acc}")
    case msg do
      {:packet, data} ->
        Logger.error("framer_func got: #{inspect msg}, acc: #{inspect acc}")
        items = String.split(acc <> data, @seperator, trim: false);
        {last_item, lines} = List.pop_at(items, -1, "");

        frames = lines |> Enum.map(&frame/1)
        if byte_size(last_item) < @max_buffer do
          {[frames], last_item} # the last line is always the last item
        else
          {[frames, [last_item |> frame(:partial) ]], ""} # overflow, push out packet
        end
      :timeout ->
        if acc == "" do
          {[[]], ""}
        else
          {[[acc |> frame(:partial)]], ""}
        end
    end
  end

  def dispatch_type({_type, _term} = msg) do
    # Logger.error("parser: dispatch: #{inspect(msg)}")
    GenServer.cast(PjonElixirSerial.DeviceManager, {:route, msg})
  end

  def receive_data_packet() do
    receive do
      {:packet, bindata} = msg ->
        # Logger.error("parser: receive_data_packet: #{inspect(msg)}")
        {:packet, bindata}
    after
      @timeout ->
        # Logger.error("receive_data_packet: timeout")
        :timeout
    end
  end

end
