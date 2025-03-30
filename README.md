# 🌴 Jungle Adventure

**Jungle Adventure** là một tựa game 2D side-scrolling chiến đấu được phát triển bằng C++ và SDL2. Người chơi sẽ hóa thân thành một Warrior hoặc Archer để chiến đấu chống lại các boss đáng sợ trong rừng sâu, thu thập xu, mua vật phẩm, và giải cứu khu rừng khỏi thế lực bóng tối!

---

## 🚀 Tính năng nổi bật

- 🎮 **Chọn nhân vật**:
  - **Warrior**: Cận chiến, kỹ năng Rage tăng sức mạnh và khả năng đỡ đòn.
  - **Archer**: Tầm xa, tốc độ cao, có kỹ năng Roll và Rage tăng tốc độ bắn.

- 🐉 **Boss chiến hấp dẫn**:
  - **Flying Demon**: Tấn công từ xa với fireball.
  - **Demon Slime**: Slime khổng lồ triệu hồi thiên thạch, cận chiến mạnh mẽ.

- 🛒 **Cửa hàng vật phẩm**:
  - Hồi máu, tăng sát thương, tăng tốc độ, tạo khiên chắn sát thương.

- 🧠 **Hệ thống Rage**: Khi đánh trúng hoặc bị thương, thanh Rage tăng lên để kích hoạt kỹ năng đặc biệt.

- 💥 **Hiệu ứng**: Va chạm, rung màn hình, âm thanh sống động.

---

## 🎮 Cách chơi

| Phím | Chức năng                                  |
|------|---------------------------------------------|
| A/D  | Di chuyển trái/phải                         |
| SPACE | Nhảy                                       |
| J    | Tấn công (Warrior) / Bắn tên (Archer)       |
| L    | Đỡ đòn (Warrior) / Lăn né (Archer)          |
| I    | Kích hoạt Rage                              |
| ESC  | Tạm dừng                                    |
| H    | Hiện/tắt bảng hướng dẫn điều khiển          |

---

## 🛠️ Hướng dẫn cài đặt & chạy game

### Yêu cầu:
- Hệ điều hành: Windows / Linux
- Cài sẵn: `SDL2`, `SDL2_image`, `SDL2_mixer`, `SDL2_ttf`
- Trình biên dịch C++ hỗ trợ C++11 trở lên

### Trên Linux:
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
g++ main.cpp -o JungleAdventure -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
./JungleAdventure
```

### Trên Windows:
- Cài đặt SDL2 và các thư viện đi kèm.
- Dùng IDE như Visual Studio, Code::Blocks
- Thêm thư viện `.lib` và include đúng thư mục `SDL2`, cấu hình path đầy đủ.
- Build và chạy.

---

## 📁 Cấu trúc thư mục

```
├── main.cpp               # Entry point
├── game.h                 # Điều khiển toàn bộ trò chơi
├── graphics.h             # Xử lý vẽ và texture
├── archer.h / warrior.h   # 2 lớp nhân vật có thể chơi
├── flyingdemon.h / demonslime.h  # Các boss chính
├── asteroid.h             # Thiên thạch do Demon Slime tạo ra
├── controls.h             # Điều khiển phím
├── defs.h                 # Hằng số định nghĩa toàn cục
├── assets/                # Hình ảnh, nhạc, âm thanh, font
└── README.md             
```

---

## 🏆 Mục tiêu trò chơi

- Chọn nhân vật phù hợp với phong cách của bạn.
- Vượt qua từng stage và đánh bại boss cuối cùng.
- Mua vật phẩm từ shop để nâng cấp sức mạnh.
- Trở thành huyền thoại của khu rừng!

---
