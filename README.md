# Rap_chieu_phim
# 🎬 HỆ THỐNG ĐẶT VÉ THÔNG MINH CLI (TICKET BOOKING SYSTEM)

## 📝 Mô tả ngắn gọn
Hệ thống Đặt Vé CLI là một ứng dụng giả lập quản lý đặt ghế, hủy vé và xử lý danh sách chờ tự động cho Rạp chiếu phim hoặc Xe khách. Ứng dụng tập trung vào việc tối ưu hóa hiệu năng bằng cách kết hợp nhiều cấu trúc dữ liệu nâng cao, giúp việc tìm kiếm, phân phối ghế và quản lý lịch sử giao dịch diễn ra với độ phức tạp thuật toán thấp nhất.

---

## 🛠️ Cấu trúc dữ liệu sử dụng

* **BST (Binary Search Tree - Cây nhị phân tìm kiếm):** * *Dùng để:* Lưu trữ và quản lý danh sách khách hàng đã đặt vé thành công theo mã định danh (ID).
    * *Vì:* BST cho phép tìm kiếm, chèn mới thông tin khách hàng cực nhanh với độ phức tạp trung bình là $O(\log N)$, tối ưu hơn hẳn so với việc duyệt mảng tuần tự $O(N)$.
* **Priority Queue (Hàng đợi ưu tiên):** * *Dùng để:* Quản lý danh sách chờ khi rạp/xe đã hết ghế trống.
    * *Vì:* Cho phép tự động phân loại và ưu tiên quyền mua vé trước cho các đối tượng đặc biệt (Khách VIP, Người cao tuổi, Trẻ em) dựa trên điểm số ưu tiên mà không cần sắp xếp lại toàn bộ danh sách.
* **Stack (Ngăn xếp):** * *Dùng để:* Lưu trữ lịch sử các giao dịch vừa thực hiện (Đặt vé / Hủy vé).
    * *Vì:* Stack hoạt động theo cơ chế LIFO (Vào sau ra trước), rất hoàn hảo để triển khai tính năng **Hoàn tác (Undo)** tác vụ gần nhất của hệ thống.
* **2D Array (Mảng 2 chiều):** * *Dùng để:* Biểu diễn sơ đồ phân bố ghế ngồi trong rạp (Dạng hàng và cột).
    * *Vì:* Giúp truy xuất trực tiếp trạng thái của một ghế bất kỳ qua chỉ số phòng vé với độ phức tạp $O(1)$ và dễ dàng kết xuất ra giao diện trực quan (ASCII).

---

## 🚀 Compile và chạy

Hệ thống hỗ trợ chạy đa nền tảng (Windows, Linux, macOS) và tự động tối ưu hóa bắt phím trực tiếp. Di chuyển các file vào đúng cấu trúc thư mục `src/` và chạy lệnh sau tại thư mục gốc:

```bash
g++ -std=c++17 src/main.cpp src/functions.cpp -o app && ./app
