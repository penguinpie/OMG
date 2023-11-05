// main.dart file full code
import 'dart:math' as Math;
import 'package:flutter/material.dart';

class JoyStick extends StatefulWidget {
  final double radius;
  final double stickRadius;
  final Function callback;

  const JoyStick(
      {Key? key,
      required this.radius,
      required this.stickRadius,
      required this.callback})
      : super(key: key);
  @override
  _JoyStickState createState() => _JoyStickState();
}

class _JoyStickState extends State<JoyStick> {
  final GlobalKey _joyStickContainer = GlobalKey();
  double yOff = 0, xOff = 0; //조이스틱 위젯의 위치
  double _x = 0, _y = 0; //조이스틱의 현재 위치
  @override
  //위젯의 상태 초기화 시 호출
  void initState() {
    super.initState();
    //widgetbinding.instance.add ~
    //프레임이 랜더링된 후에 실행되어야하는 작업을 등록
    //addPostFrameCallback
    //현재 앱의 widgetBinding 인스턴스에 콜백함수를 등록. 등록된 콜백은 프레임이 랜더링된 이후 한번 호출
    WidgetsBinding.instance.addPostFrameCallback((timeStamp) {
      //key에서 위젯의 renderbox 정보를 얻어온다. renderbox에는 위젯의 크기와 위치정보가 존재
      //buildContext 검색
      final RenderBox renderBoxWidget =
          _joyStickContainer.currentContext?.findRenderObject() as RenderBox;
      //화면전체좌표계에서 랜더박스의 좌표를 얻어온다. localtoglobal은 위젯의 로컬좌표를 화면 전체좌표로 변환한다.
      //offset.zero 는 랜더 이미지의 원점좌표이고 localToGlobal을 이용하여 전체화면 좌표로 변환한다.
      //https://letyarch.blogspot.com/2021/06/fluttersizepos.html
      //아마 offset은 중앙 좌표가 아니고 위젯의 top left 좌표일 것이다
      //top left 를 (0,0)으로 설정하고 global 좌표로 매핑한다.
      final offset = renderBoxWidget.localToGlobal(Offset.zero);
      //여기 x0ff와 y0ff는 배경위젯 기준 top left 기준 좌표이다.
      xOff = offset.dx; //조이스틱 위젯의 위치를 local에서 global로 변환한 후의 x값
      yOff = offset.dy; //조이스틱 위젯의 위치를 local에서 global로 변환한 후의 y값
    });
    _centerStick();
  }
  // 정리하면, 위젯의 글로벌 키를 미리 생성시킨 후 위젯이 생성될 때 실행되는 initState 함수를 override 한다.
  // custum된 initstate 함수는 기존의 initstate 함수에 프레임이 랜더링 된후 실행될 콜백 함수를 하나 정의한다.
  // callback 함수 내용
  // 그 콜백 함수에선 key에 존재하는 currentContext에서 랜더링 이미지에 대한 정보를 받고, 그 정보중에서 랜더링 이미지의 local 좌표를 확인한다.
  // 그 좌표를 localtoglobal 함수를 이용하여 전체화면에 대한 좌표로 변환하고 x0ff, y0ff애 저장한다
  // 또한 centerStick 함수를 실행한다.

  // _centerStick 함수 또한 위젯이 초기화할때 실행되는 함수이다
  // widget.radius는 배경위젯의 반지름을 의미한다
  // widget은 main.dart에서 joystick 위젯을 생성할 때, 위에서 joystick class를 생성할때
  // radius, stickRadius, callback 함수를 매개변수로 받는다
  // 이 함수는 위젯이 초기화 될때마다 조이스틱의 현재위치 _x, _y를 top left기준 배경위젯의 정중앙으로
  // 초기화 하여 조이스틱 위젯이 항상 중앙에 위치하도록 조정한다
  // 그 후 초기화된 좌표를 sendCoordinates 함수를 이용하여 송신한다.
  void _centerStick() {
    setState(() {
      // _x, _y = 반지름을 통하여 stick이 배경 위젯 정가운데에 정렬되도록 한다.
      _x = widget.radius;
      _y = widget.radius;
    });
    _sendCoordinates(_x, _y);
  }

  // ?
  int map(x, inMin, inMax, outMin, outMax) {
    return ((x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin).floor();
  }

  // onPointerMove 함수는 화면에 터치가 발생했을때 state를 설정하는 함수이다.
  // PointerEvent라는, 화면에 터치이벤트가 발생할때 생성되는 변수 details를 매개변수로 받는다.
  // final_x, final_y는 details가 가지고 있는 현재위치에서 기존 조이스틱위젯의 top left 기준 global좌표
  // x0ff y0ff를 빼서 위젯내에서 상대적인 이동위치를 계산한다.
  // if 문과 isStickInside 함수를 이용하여 조이스틱이 위젯안에 존재하는 지를 판단한다.
  // 만약 조이스틱이 배경위젯 안에 있다면 현재 상태를 setState 함수를 이용하여 _x, _y 에 저장한다.
  // _x, _y 는 배경 위젯 중앙에서 이동한 거리이자 배경위젯을 기준으로 스틱이 존재하는 상대위치이다.
  // 그리고 그 값을 sendCoordinate 함수를 이용하여 송신한다.
  // widget.radius는 배경위젯의 중앙 좌표이다.
  // 0ffset은 이동한 조이스틱의 global 좌표이다.
  // detail.position은 아마도 global 기준으로 매핑될 것이다.
  void _onPointerMove(PointerEvent details) {
    // 위젯 내에서 상대위치
    final x = details.position.dx - xOff;
    final y = details.position.dy - yOff;
    if (isStickInside(x, y, widget.radius, widget.radius,
        widget.radius - widget.stickRadius)) {
      // x and y cordinates
      setState(() {
        _x = x;
        _y = y;
      });
      _sendCoordinates(x, y);
    }
  }

  // onPointerUp 함수는 화면에서 터치가 사라졌을때 state를 설정하는 함수이다.
  // PointerUpEvent라는, 화면에서 터치가 사라졌을때 생성되는 변수 event를 매개변수로 받는다.
  // 만약 터치가 화면에서 사라졌을 경우, _centerStick 함수를 이용하여 스틱을 중앙에 배치한다.
  void _onPointerUp(PointerUpEvent event) {
    _centerStick();
  }

  // 이 코드는 조이스틱의 현재 이동속도와 방향을 계산하고, 이를 콜백함수를 통해 전달한다.
  // widget.radius 는 배경위젯의 정중앙을 의미한다.
  // x, y 는 스틱이 배경위젯 중앙을 기준으로 이동한 거리이다.
  void _sendCoordinates(double x, double y) {
    // x와 y는 배경 위젯 top left를 기준으로 측정된 좌표이므로 해당 값을 위젯 중앙에 대한 좌표로 변환해야한다.
    double speed = y - widget.radius;
    double direction = x - widget.radius;

    var vSpeed = -1 *
        map(speed, 0, (widget.radius - widget.stickRadius).floor(), 0, 100);
    var vDirection =
        map(direction, 0, (widget.radius - widget.stickRadius).floor(), 0, 100);

    widget.callback(vDirection, vSpeed);
  }

  // x, y는 top left 기준 위젯 내애서 상대위치
  // 공식맞음 그림그려보셈
  isStickInside(x, y, circleX, circleY, circleRadius) {
    var absX = Math.pow((x - circleX).abs(), 2.0);
    var absY = Math.pow((y - circleY).abs(), 2.0);
    return Math.sqrt(absX + absY) < circleRadius;
  }

  // child를 갖는 위젯 : container, gesturDetector, SizeBox (한개의 위젯만 추가가능)
  // children을 갖는 위젯 : Column, Row, ListView, Stack (여러개의 위잿을 추가가능)
  // https://parkjh7764.tistory.com/168
  @override
  Widget build(BuildContext context) {
    return Center(
      child: Listener(
        behavior: HitTestBehavior.opaque,
        onPointerMove: _onPointerMove,
        onPointerUp: _onPointerUp,
        child: Container(
          key: _joyStickContainer,
          width: widget.radius * 2,
          height: widget.radius * 2,
          decoration: BoxDecoration(
            borderRadius: BorderRadius.circular(widget.radius),
            color: Colors.grey.shade800,
          ),
          child: Stack(
            children: [
              Positioned(
                left: _x - widget.stickRadius,
                top: _y - widget.stickRadius,
                child: Container(
                  width: widget.stickRadius * 2,
                  height: widget.stickRadius * 2,
                  decoration: BoxDecoration(
                    color: Colors.lightBlue,
                    borderRadius: BorderRadius.circular(widget.stickRadius),
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
