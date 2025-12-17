# ESP32-C3 SuperMini 핀맵 예시

## MAVLink(UART1)
- RX: GPIO20 (Pixhawk TX에 연결)
- TX: GPIO21 (Pixhawk RX에 연결)
- Baud: 230400 (스케치/펌웨어와 일치)

## 센서/무선(옵션)
### NRF24L01
- CE: GPIO10
- CSN: GPIO9
- SPI 핀은 보드 기본값 사용(필요 시 `SPI.begin(SCLK, MISO, MOSI, SS)`로 재지정)

### LoRa (SX127x, 예시)
- SS/NSS: 보드 가용 핀 지정(예: GPIO7)
- RST/DIO0: 가용 GPIO에 배치
- SPI: 보드 기본값 사용 또는 재지정

> ESP32-C3 보드마다 SPI 기본 핀 배치가 다를 수 있습니다. 보드 문서/Variants를 확인하여 실제 배치로 맞춰 주세요.

## 참고
- 전 모든 신호는 3.3V TTL
- 외부 모듈 전원(전류)에 주의
- 충돌 최소화를 위해 UART/SPI 공유 시 타이밍 분리(예: 스위칭) 권장
