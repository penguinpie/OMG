import 'dart:async';

import 'package:web_socket_channel/io.dart';
import 'package:web_socket_channel/status.dart' as status;
import 'package:web_socket_channel/web_socket_channel.dart';

class WebSocket {
  // ------------------------- Members ------------------------- //
  late String url;
  WebSocketChannel? _channel;
  StreamController<bool> streamController = StreamController<bool>.broadcast();
  final channel = IOWebSocketChannel.connect('');
  // ---------------------- Getter Setters --------------------- //
  String get getUrl {
    return url;
  }

  set setUrl(String url) {
    this.url = url;
  }

  Stream<dynamic> get stream {
    if (_channel != null) {
      return _channel!.stream;
    } else {
      throw WebSocketChannelException("The connection was not established !");
    }
  }

  // --------------------- Constructor ---------------------- //
  WebSocket(this.url);

  // ---------------------- Functions ----------------------- //

  /// Connects the current application to a websocket
  void connect() async {
    _channel = WebSocketChannel.connect(Uri.parse("ws://192.168.246.223:5000"));
  }

  /// Disconnects the current application from a websocket
  void disconnect() {
    if (_channel != null) {
      _channel!.sink.close(status.goingAway);
    }
  }

  void sendData(data) {
    channel.sink.add(data);
  }
}
