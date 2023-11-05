import 'package:flutter/material.dart';
import 'VideoStreaming.dart';

// 메인실행코드!
// MyApp -> 우리가 실행할 어플 입니다.
void main() {
  runApp(const MyApp());
}

// MyApp을 여기서 정의합니다.
// 플러터는 위젯 클래스로 어플을 구성하는데, 여기선 statelessWidget를 상속받아서 구현했습니다!
class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // @override는 상속받은 클래스에서 덮어씌울 내용을 선언하는 것입니다.
  // 여기서는 statelesswidget 클래스에서 widget build 부분, 즉 위젯의 구성을 덮어씌워 우리가 원하는 구성으로 변경할 것입니다.
  // 위에서 플러터는 위젯으로 어플을 구현한다 했죠? 아래코드를 보면
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      // 빌드의 결과물로 MaterialApp이라는 위젯을 생성할 겁니다
      title:
          'Flutter Demo', // 그 위젯을 구성하는 부분이 여기부터 작성되어있는데, 위젯을 title, theme, home으로 구성했습니다.
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home:
          const VideoStream(), // 여기서 이 부분이 핵심인데, 플러터는 위젯안에 위젯을 집어넣을 수가 있습니다. 그래서 MaterialApp이라는 위젯의 home 부분에 videoStream 위젯을 삽입했습니다.
      // 위젯은 오픈소스도 있고, 코드로 구현할 수도 있는데, 저는 구현한 코드를 import하여 사용할 예정입니다. 코드 맨 위에 import 'VideoStreaming.dart'; 이 코드로 위젯을 가져온겁니다!
    );
  }
}
