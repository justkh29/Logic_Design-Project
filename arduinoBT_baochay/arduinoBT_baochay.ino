int pinA0 = A0;
int pinD0 = 0;
int buzzer = 13;
int flameA0;

// Biến để lưu thời gian trôi qua
unsigned long previousMillis = 0;
const long interval = 20;

void setup() {
  // Khởi tạo Serial để kiểm tra kết quả
  Serial.begin(9600);

  // Cài đặt các chân
  pinMode(pinA0, INPUT);
  pinMode(pinD0, INPUT); // Cảm biến D0 không được sử dụng trong mã này
  pinMode(buzzer, OUTPUT);
}

void loop() {
  // Kiểm tra xem có đủ thời gian để kiểm tra ngọn lửa chưa
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Lưu lại thời gian hiện tại
    previousMillis = currentMillis;

    // Gọi hàm kiểm tra ngọn lửa và điều khiển buzzer
    kiểmTraNgonLua(pinA0, buzzer);
  }

  // Các tác vụ khác có thể được thêm vào ở đây
}

void kiểmTraNgonLua(int pin, int buzzerPin) {
  // Đọc giá trị từ cảm biến ngọn lửa
  flameA0 = analogRead(pin);
  
  // Kiểm tra nếu mức cảm biến dưới ngưỡng
  if (flameA0 < 100) {
    digitalWrite(buzzerPin, HIGH);  // Bật còi
  } else {
    digitalWrite(buzzerPin, LOW);   // Tắt còi
  }

  // In giá trị cảm biến ra Serial Monitor
  Serial.print("Flame Sensor Value: ");
  Serial.println(flameA0);
}
