# Desktop Serial

Phần mềm truyền nhận serial giống Hercules.

*Build command*

```bash
cmake -B . -S .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release && make
```

---

## Quản lý cài đặt bằng script

Dự án cung cấp script `desktop-serial-manager.sh` để:

* Cài đặt ứng dụng
* Nâng cấp version
* Gỡ bỏ hoàn toàn
* Kiểm tra trạng thái

Script hỗ trợ build + cài đặt tự động trên Ubuntu.

---

## Yêu cầu

* Ubuntu (khuyến nghị 22.04+)
* Có `sudo`
* Có internet để cài dependency

---

## Cấp quyền chạy

```bash
chmod +x desktop-serial-manager.sh
```

---

## 1. Cài đặt

```bash
./desktop-serial-manager.sh install <file-zip> [version]
```

Ví dụ:

```bash
./desktop-serial-manager.sh install desktop-serial.zip
```

Hoặc chỉ định version:

```bash
./desktop-serial-manager.sh install desktop-serial.zip 1.0.0
```

### Kết quả sau khi cài

* Binary:

  ```
  /opt/desktop-serial-<version>/desktop_serial
  ```

* Symlink:

  ```
  /opt/desktop-serial -> version hiện tại
  ```

* Command:

  ```
  desktop-serial
  ```

* Launcher:

  ```
  xuất hiện trong menu ứng dụng
  ```

---

## 2. Nâng cấp

```bash
./desktop-serial-manager.sh upgrade <file-zip> [version]
```

Ví dụ:

```bash
./desktop-serial-manager.sh upgrade desktop-serial.zip 1.1.0
```

### Cách hoạt động

* Build version mới
* Cài vào:

  ```
  /opt/desktop-serial-<new-version>
  ```
* Cập nhật symlink:

  ```
  /opt/desktop-serial -> version mới
  ```

Không ảnh hưởng version cũ → có thể rollback

---

## 3. Kiểm tra trạng thái

```bash
./desktop-serial-manager.sh status
```

Hiển thị:

* Version hiện tại
* Binary path
* Launcher trạng thái
* Danh sách version đã cài

---

## 4. Gỡ cài đặt

```bash
./desktop-serial-manager.sh uninstall
```

Xóa:

* Toàn bộ `/opt/desktop-serial-*`
* Symlink `/opt/desktop-serial`
* Command `/usr/local/bin/desktop-serial`
* Launcher `.desktop`
* Icon

---

## 5. Rollback về version cũ

Ví dụ quay lại `1.0.0`:

```bash
sudo ln -sfn /opt/desktop-serial-1.0.0 /opt/desktop-serial
sudo ln -sfn /opt/desktop-serial/desktop_serial /usr/local/bin/desktop-serial
```

---

## 6. Cách chạy

### Từ terminal

```bash
desktop-serial
```

### Từ GUI

* Mở Applications
* Tìm: **Desktop Serial**

---

## 7. Lưu ý

* Script sẽ tự cài dependency:

  * `qt6-base-dev`
  * `qt6-serialport-dev`
  * `cmake`, `ninja`, ...
* Nếu icon không cập nhật:

  ```bash
  gtk-update-icon-cache -f /usr/share/icons/hicolor
  ```

---

## 8. Lỗi thường gặp

### Không tìm thấy package

```bash
Package libqt6serialport6-dev is not available
```

Fix: script đã dùng đúng package:

```bash
qt6-serialport-dev
```

---

## 9. Cấu trúc cài đặt

```text
/opt/
 ├── desktop-serial-1.0.0
 ├── desktop-serial-1.1.0
 └── desktop-serial -> symlink

/usr/local/bin/
 └── desktop-serial

/usr/share/applications/
 └── desktop-serial.desktop
```
