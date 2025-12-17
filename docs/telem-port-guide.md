# TELEM 포트 연결/설정 가이드

## 핀아웃(일반적 구성)
- 6핀: VCC / TX / RX / CTS / RTS / GND
- 기본 연결: TX↔RX 교차, GND 공통 (CTS/RTS 미사용)

## 레벨/전원
- 전압 레벨: 3.3V TTL
- 전원: TELEM 5V는 보드 구동 전류 한도 확인 후 사용(권장: 외부 전원)

## ArduPilot 설정(예: TELEM1)
- `SERIAL1_PROTOCOL = 2` (MAVLink2)
- `SERIAL1_BAUD = 230400` (스케치와 동일하게)
- 필요 시 SRx_* 스트림 비율 조정

## PX4 설정
- `MAV_0_CONFIG = TELEM1`
- `MAV_0_BAUD = 230400`

## 트러블슈팅
- 통신 없음: Baud/프로토콜/케이블/커넥터 확인
- 한 방향만 보임: TX/RX 교차 재확인
- 노이즈/깨짐: GND 공통, 케이블 길이/품질 점검, Baud 하향 테스트
