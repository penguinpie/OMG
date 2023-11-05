// Main Widget
import 'dart:convert';
import 'dart:developer';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:omg_new/flutter_joystick.dart';

import 'constants.dart';
import 'styles.dart';
import 'websocket.dart';

// 비디오 스트리밍 위젯을 구현했습니다.
// 원래는 개별위젯을 조립해서 main에 예쁘게 정리하는게 정석이지만, 정리가 귀찮아서 그냥 비디오 스트리밍 위젯에 조이스틱 위젯을 삽입했습니다.......
class VideoStream extends StatefulWidget {
  const VideoStream({Key? key}) : super(key: key);

  @override
  State<VideoStream> createState() => _VideoStreamState();
}

class _VideoStreamState extends State<VideoStream> {
  final WebSocket _socket = WebSocket(Constants.videoWebsocketURL);
  bool _isConnected = false;
  void connect(BuildContext context) async {
    _socket.connect();
    setState(() {
      _isConnected = true;
    });
  }

  void disconnect() {
    _socket.disconnect();
    setState(() {
      _isConnected = false;
    });
  }
// 여기까지가 비디오스트리밍 위젯의 기능을 구현한 것이고,
// 아래는 위에서 구현한 기능을 시각화하여 위젯의 구성부분을 조립하는 것입니다.
// 아마 오류갸 발생할 건데 제가 체크안하고 올려서 에러난 코드가 있습니다. 주석처리해주세요!

  // 여기에 x,y websocket을 보내는 코드 포팅하면된다.

  void callback(x, y) {
    log('callback x => $x and y $y');
    _socket.sendData('x: $x y : $y');
  }

  // 위젯을 구성부분입니다
  // 아까는 statelesswidget을 상속하여 구현했다면, 여기서는 statgefulwidget 클래스를 상속하여 구현했습니다.
  // (플러터는 statelesswidget, statefulwidget 이렇게 2개의 기본 구성 위젯을 제공합니다)
  // 아까와 마찬가지로 기본 위젯 클래스에서 빌드하는 부분을 @override로 덮어씌웁니다
  @override
  Widget build(BuildContext context) {
    // 마찬가지로 위젯을 빌드하는데를 새로 작성
    return Scaffold(
      // 이번에는 scaffold라는 위젯 구성을 사용할 것입니다.
      appBar: AppBar(
        // scaffold 위젯의 구성 -> appbar부분, body부분,
        title: const Text("???"),
        // 또한 appbar는 title로 구성되어 있고,
      ),
      body: Padding(
        // body는 padding 안에 center안에 column안에 row안에 위젯을 삽입합니다.
        // 조이스틱 위젯은 코드의 116 줄에 있습니다!
        padding: const EdgeInsets.all(20.0),
        child: Center(
          child: Column(
            children: [
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  ElevatedButton(
                    onPressed: () => connect(context),
                    style: Styles.buttonStyle,
                    child: const Text("Connect"),
                  ),
                  ElevatedButton(
                    onPressed: disconnect,
                    style: Styles.buttonStyle,
                    child: const Text("Disconnect"),
                  ),
                ],
              ),
              const SizedBox(
                height: 50.0,
              ),
              _isConnected
                  ? StreamBuilder(
                      stream: _socket.stream,
                      builder: (context, snapshot) {
                        if (!snapshot.hasData) {
                          return const CircularProgressIndicator();
                        }

                        if (snapshot.connectionState == ConnectionState.done) {
                          return const Center(
                            child: Text("Connection Closed !"),
                          );
                        }
                        //? Working for single frames
                        return Image.memory(
                          Uint8List.fromList(
                            base64Decode(
                              (snapshot.data.toString()),
                            ),
                          ),
                          gaplessPlayback: true,
                          excludeFromSemantics: true,
                        );
                      },
                    )
                  : const Text("Initiate Connection"),
              const Spacer(),
              JoyStick(
                  radius: 100,
                  stickRadius: 30,
                  callback: callback) // <- 조이스틱 위젯입니다!!
              // 정리하자면, 글쓴이의 귀차니즘으로 비디오 스트리밍 위젯에 어거지로 조이스틱 위젯을 구겨넣었습니다..ㅎ
              // 그래서 main.dart를 실행하려면 videostreaming.dart, joystick.dart, main.dart 3개의 파일이 있어야합니다!
            ],
          ),
        ),
      ),
    );
  }
}
