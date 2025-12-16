// Thư viện
#include <IRremote.hpp>
// Dòng này nạp thư viện IRremote – dùng để gửi và nhận tín hiệu hồng ngoại (IR) giữa các thiết bị (ví dụ: điều khiển TV, LED IR, v.v.).

const int IR_SEND_PIN = 3;  // Pin dùng để gửi IR Signal
// Khai báo hằng số IR_SEND_PIN gán giá trị 3, nghĩa là chân số 3 của Arduino được dùng để phát tín hiệu hồng ngoại (IR Signal).
bool turnOn = true;
// Khai báo biến logic (boolean) turnOn, ban đầu là true, nghĩa là trạng thái khởi đầu là bật LED (hoặc gửi mã bật).

const unsigned long LED_ON_CODE = 0x1; // Định nghĩa một hằng số để mã dễ hiểu hơn
const unsigned long LED_OFF_CODE = 0x0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // Khởi tạo bộ phát tín hiệu hồng ngoại, chỉ định:
  IrSender.begin(IR_SEND_PIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  // transmit the IR signal
  if (turnOn) {
    IrSender.sendRC5(LED_ON_CODE, 8);
    Serial.println("Sent RC5 ON Code");
    Serial.println(LED_ON_CODE);
  } else {
    IrSender.sendRC5(LED_OFF_CODE, 8);
    Serial.println("Sent RC5 OFF Code");
    Serial.println(LED_OFF_CODE);
  }

  turnOn = !turnOn; // Đảo trạng thái bật/tắt
  delay(2000);      // Chờ 2 giây trước khi gửi tín hiệu tiếp theo
}