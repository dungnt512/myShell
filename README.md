# myShell - Shell đơn giản cho Windows

Shell đơn giản được phát triển để nghiên cứu các API quản lý tiến trình trong Windows.

## Tính năng

1. **Chế độ thực thi lệnh**:
   - Foreground: shell đợi tiến trình kết thúc (`command`)
   - Background: shell và tiến trình chạy song song (`command &`)

2. **Lệnh quản lý tiến trình**:
   - `list`: Hiển thị danh sách tiến trình do shell quản lý
   - `kill <pid>`: Kết thúc một tiến trình
   - `stop <pid>`: Tạm dừng một tiến trình
   - `resume <pid>`: Tiếp tục một tiến trình

3. **Lệnh tích hợp**:
   - `exit`: Thoát shell
   - `help`: Hiển thị trợ giúp
   - `date`: Hiển thị ngày hiện tại
   - `time`: Hiển thị giờ hiện tại
   - `dir [path]`: Liệt kê nội dung thư mục
   - `path`: Hiển thị biến môi trường PATH
   - `addpath <path>`: Thêm đường dẫn vào PATH

4. **Phím tắt**:
   - `Ctrl+C`: Dừng tiến trình foreground đang chạy

5. **Thực thi file batch**:
   - Shell tự động nhận diện và thực thi file `.bat`

## Biên dịch

Để biên dịch shell, sử dụng lệnh:

```
make
```

Để xóa các file object và executable:

```
make clean
```

## Sử dụng

Sau khi biên dịch, chạy shell bằng lệnh:

```
myShell hoặc .\myShell
```

## Cấu trúc mã nguồn

- `main.c`: Vòng lặp shell chính, xử lý tín hiệu
- `process.c/h`: Quản lý tiến trình và thực thi lệnh
- `commands.c/h`: Xử lý các lệnh tích hợp
- `utils.c/h`: Các hàm tiện ích

## Lưu ý

Shell này được phát triển với mục đích giáo dục để hiểu cách hoạt động của các API quản lý tiến trình trong Windows, không phải để thay thế các shell đầy đủ tính năng như PowerShell hay Command Prompt. 