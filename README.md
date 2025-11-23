# IOT 기반 실시간 침입 감지 시스템
<img width="567" height="370" alt="image" src="https://github.com/user-attachments/assets/1e088cd3-ac00-442e-9318-a3c93850b580" />


## 📖 프로젝트 개요
무단 침입 및 원격 개방 등 출입 통제 실패로 인한 보안 위협을 방지하기 위해  
**버튼 센서 + Wi-Fi + Bluetooth 기반의 실시간 출입 감지 시스템**을 구현했습니다.  

- 버튼 센서를 통한 출입 감지  
- LCD, Buzzer, LED를 활용한 침입 알림  
- 관리실에서 직관적으로 출입 상태 파악 및 즉각 대응 가능

---

## ⚙️ 개발 환경
- **측정부**: Button Sensor, STM32, HC-06 (Bluetooth), ESP-01 (Wi-Fi)  
- **메인 서버**: Raspberry Pi, MariaDB, PHP 기반 웹 서버  
- **동작부**: Arduino, LCD, Buzzer, LED  

---

## 🛠️ 시스템 구성도
1. **Button Sensor 1**: DOOR OPEN 감지 (침입 발생)  
2. **Button Sensor 2**: 관리자 CLOSE (경보 해제)  
3. **STM32**: 센서 신호 감지 및 Wi-Fi 전송  
4. **ESP-01**: Raspberry Pi로 데이터 전송  
5. **Raspberry Pi**: DB 저장 및 웹 서버 운영  
6. **HC-06**: Bluetooth → Arduino  
7. **Arduino**: LCD, Buzzer, LED 알림 표시  

---

## 🔄 구현 과정
- **STM32**: 인터럽트 신호 감지 후 Wi-Fi로 명령 전송  
- **Raspberry Pi**: PHP + MariaDB 기반 DB 저장 (출입 로그 기록)  
- **Arduino**: 3초마다 최신 DB 요청 → LCD, Buzzer, LED 상태 갱신  

---

## 🎥 결과 및 시연
- 침입 발생 시 LCD, Buzzer, LED를 통한 경고 알림  
- 관리실에서 실시간 출입 상태 확인 가능  

---

## 🔍 고찰 및 개선 사항
- 버튼 센서의 물리적 한계 → **초음파 센서 교체 필요**  
- 단순한 데이터 기록 구조 → **세부 출입 주체 및 패턴 분석 필요**  
- BLE 비콘 기반 RSSI 거리 판단 불안정 → **향후 구현 예정**  
- 관리자 알림만 제공 → **스마트폰 연동 통한 실시간 알림 확장 필요**

---

## 🙏 감사합니다
