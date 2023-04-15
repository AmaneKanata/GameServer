# GameServer
C++로 작성된 TCP 기반의 실시간 동기화 서버입니다.
<u>[이 링크](https://crystal-meeting-39a.notion.site/f8d52a816f4049a190f7dce1d341a541)</u>에서 게임 서버에 대한 설명을 확인하실 수 있습니다.

## 작동 환경
- C++ 17
- Boost 1.81.0
- Google Protobuf 3.21.12

## 빌드 방법
cmake를 이용해 빌드를 수행합니다.
### Windows
필요한 모든 헤더 파일 및 라이브러리가 리포지토리에 포함되어 있습니다. Visual Studio에서 별도의 설정 없이 바로 빌드할 수 있습니다.
### Linux
1. 먼저 위에서 언급한 라이브러리를 설치합니다.
2. 루트 폴더에서 다음 명령을 실행합니다:
```
cmake .
make
```
이후 `src/Server` 아래에 실행 파일이 생성됩니다.
