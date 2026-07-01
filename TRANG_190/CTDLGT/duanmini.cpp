#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <iomanip>

using namespace std;

// ==========================================================================
// 1. ĐỊNH NGHĨA CÁC CẤU TRÚC DỮ LIỆU TRỌNG TÂM (STRUCT)
// ==========================================================================

// Cấu trúc lưu trữ thông tin của một Khách hàng
struct Customer {
    string id;         // Mã định danh duy nhất (Ví dụ: KH001)
    string name;       // Họ và tên khách hàng (Viết không dấu để tránh lỗi font)
    int priority;      // Mức độ ưu tiên: 3: VIP, 2: Nguoi cao tuoi, 1: Thuong
    int bookedRow;     // Vị trí hàng ghế đã đặt (Mặc định = -1 nếu chưa có ghế)
    int bookedCol;     // Vị trí cột ghế đã đặt (Mặc định = -1 nếu chưa có ghế)

    // Hàm khởi tạo mặc định (Constructor không tham số)
    Customer() : id(""), name(""), priority(1), bookedRow(-1), bookedCol(-1) {}

    // Hàm khởi tạo có tham số để gán nhanh dữ liệu khi tạo mới khách hàng
    Customer(string i, string n, int p) : id(i), name(n), priority(p), bookedRow(-1), bookedCol(-1) {}
};

// Cấu trúc cấu hình tiêu chí so sánh cho HÀNG ĐỢI ƯU TIÊN (Priority Queue)
struct ComparePriority {
    bool operator()(const Customer& c1, const Customer& c2) {
        // Trả về true nếu mức ưu tiên của c1 nhỏ hơn c2
        // Cơ chế này giúp khách hàng có priority LỚN NHẤT (VIP = 3) luôn được đẩy lên đầu hàng đợi
        return c1.priority < c2.priority;
    }
};

// Cấu trúc một nút (Node) trong CÂY NHỊ PHÂN TÌM KIẾM (BST)
struct BSTNode {
    Customer customer; // Dữ liệu của nút là một cấu trúc Khách hàng
    BSTNode* left;     // Con trỏ quản lý nhánh bên trái (chứa các phần tử có ID nhỏ hơn)
    BSTNode* right;    // Con trỏ quản lý nhánh bên phải (chứa các phần tử có ID lớn hơn)

    // Hàm khởi tạo nhanh một Node khi cấp phát bộ nhớ
    BSTNode(Customer c) : customer(c), left(nullptr), right(nullptr) {}
};

// Cấu trúc bản ghi lịch sử giao dịch để lưu vào NGĂN XẾP (Stack) phục vụ tính năng Hoàn tác/Hủy vé
struct Transaction {
    string type;       // Loại giao dịch: "BOOK" (Đặt vé) hoặc "CANCEL" (Hủy vé)
    Customer customer; // Bản sao thông tin khách hàng tại thời điểm giao dịch
    int row;           // Vị trí hàng ghế xảy ra giao dịch
    int col;           // Vị trí cột ghế xảy ra giao dịch
};

// ==========================================================================
// 2. LỚP QUẢN LÝ TOÀN BỘ HỆ THỐNG ĐẶT VÉ (TICKET SYSTEM)
// ==========================================================================
class TicketSystem {
private:
    static const int ROWS = 5;       // Giới hạn sơ đồ gồm 5 hàng ghế
    static const int COLS = 5;       // Giới hạn sơ đồ gồm 5 cột ghế
    const int TICKET_PRICE = 50000;  // Đơn giá vé cố định: 50.000 VND / ghế

    // [CẤU TRÚC ĐA CHIỀU - 2D ARRAY]: Sơ đồ ma trận ghế ngồi thực tế của rạp
    // Ô trống sẽ có giá trị "", ô đã đặt sẽ lưu trực tiếp ID của khách hàng ngồi đó
    string seatMap[ROWS][COLS];
    int availableSeats;              // Biến đếm số ghế trống còn lại trong rạp
    long long totalRevenue;          // Biến tích lũy tổng doanh thu tiền vé

    // [CẤU TRÚC CÂY - BST]: Quản lý và lưu trữ toàn bộ hồ sơ khách hàng theo cấu trúc hình cây
    BSTNode* bstRoot;

    // [CẤU TRÚC HÀNG ĐỢI - PRIORITY QUEUE]: Danh sách hàng chờ tự động sắp xếp theo độ ưu tiên khách hàng
    priority_queue<Customer, vector<Customer>, ComparePriority> waitingQueue;

    // [CẤU TRÚC NGĂN XẾP - STACK]: Lưu vết lịch sử giao dịch, hỗ trợ bốc phần tử mới nhất ra để hủy (LIFO)
    stack<Transaction> txHistory;

    // --- CÁC HÀM BỔ TRỢ CHUYÊN BIỆT CHO CÂY NHỊ PHÂN TÌM KIẾM (BST) ---

    // Hàm đệ quy: Thêm một khách hàng mới vào Cây BST (Sắp xếp theo thứ tự Mã ID)
    BSTNode* insertBST(BSTNode* root, Customer c) {
        if (!root) return new BSTNode(c); // Nếu vị trí cây trống, tạo Node mới tại đây

        if (c.id < root->customer.id) {
            root->left = insertBST(root->left, c);  // Nếu ID nhỏ hơn, rẽ trái
        }
        else if (c.id > root->customer.id) {
            root->right = insertBST(root->right, c); // Nếu ID lớn hơn, rẽ phải
        }
        return root; // Trả về cây sau khi đã chèn nút thành công
    }

    // Hàm đệ quy: Tìm kiếm khách hàng trên Cây BST bằng Mã ID với tốc độ cực nhanh O(log n)
    BSTNode* searchBST(BSTNode* root, string id) {
        if (!root || root->customer.id == id) return root; // Trả về nếu tìm thấy hoặc chạm đáy cây
        if (id < root->customer.id) return searchBST(root->left, id); // Nhỏ hơn thì tìm bên trái
        return searchBST(root->right, id); // Lớn hơn thì tìm bên phải
    }

    // Hàm đệ quy: Cập nhật lại tọa độ ghế ngồi của một khách hàng đang nằm trên Cây BST
    void privateUpdateSeatInBST(BSTNode* root, string id, int r, int c) {
        if (!root) return;
        if (root->customer.id == id) {
            root->customer.bookedRow = r; // Cập nhật lại hàng
            root->customer.bookedCol = c; // Cập nhật lại cột
            return;
        }
        if (id < root->customer.id) privateUpdateSeatInBST(root->left, id, r, c);
        else privateUpdateSeatInBST(root->right, id, r, c);
    }

    // Hàm đệ quy: Duyệt cây theo thứ tự LNR (In-order) để xuất ra danh sách khách hàng xếp tăng dần theo ID
    void inOrderBST(BSTNode* root) {
        if (!root) return;
        inOrderBST(root->left); // Duyệt toàn bộ nhánh trái trước

        // Chuyển đổi mã số ưu tiên thành văn bản trực quan
        string pType = (root->customer.priority == 3) ? "VIP" : ((root->customer.priority == 2) ? "Nguoi cao tuoi" : "Thuong");

        // In dòng thông tin khách hàng hiện tại căn lề ngay ngắn
        cout << "| " << setw(10) << left << root->customer.id
            << "| " << setw(20) << left << root->customer.name
            << "| " << setw(15) << left << pType;

        // Kiểm tra xem khách hàng này đã có ghế hay đang phải xếp hàng chờ
        if (root->customer.bookedRow != -1) {
            cout << "| Ghe [" << root->customer.bookedRow + 1 << "][" << root->customer.bookedCol + 1 << "]\n";
        }
        else {
            cout << "| Dang trong hang doi cho\n";
        }

        inOrderBST(root->right); // Duyệt tiếp nhánh bên phải
    }

public:
    // Hàm khởi tạo hệ thống (Constructor thiết lập rạp trống ban đầu)
    TicketSystem() {
        availableSeats = ROWS * COLS; // Khởi tạo số ghế trống bằng diện tích ma trận (25 ghế)
        totalRevenue = 0;              // Doanh thu ban đầu bằng 0
        bstRoot = nullptr;             // Cây quản lý ban đầu trống
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                seatMap[i][j] = "";    // Gán toàn bộ ô trong mảng 2 chiều là chuỗi rỗng (Trống)
            }
        }
    }

    // CHỨC NĂNG 1: Vẽ sơ đồ ghế ngồi dạng đồ họa ký tự ASCII
    void displaySeatMap() {
        cout << "\n===== SO DO GHE NGOI ( [ . ] trong | [ X ] da dat ) =====\n\n";
        cout << "     ";
        for (int j = 0; j < COLS; j++) cout << " C" << j + 1 << "  "; // In chỉ số Cột
        cout << "\n    " << string(COLS * 5, '-') << "\n";

        // Vòng lặp quét mảng 2 chiều để dựng giao diện
        for (int i = 0; i < ROWS; i++) {
            cout << "R" << i + 1 << " |"; // In chỉ số Hàng
            for (int j = 0; j < COLS; j++) {
                if (seatMap[i][j] == "") {
                    cout << " [.] "; // Hiện dấu chấm nếu ô đó chưa ai đặt
                }
                else {
                    cout << " [X] "; // Hiện chữ X nếu ô đó đã bị chiếm giữ
                }
            }
            cout << "\n";
        }
        cout << "\nGhe trong con lai: " << availableSeats << " / " << ROWS * COLS << "\n";
    }

    // CHỨC NĂNG 2: Xử lý Đặt vé (Kết hợp Array 2D + BST + Priority Queue + Stack)
    void bookTicket(string id, string name, int priority) {
        Customer newCustomer(id, name, priority);
        bstRoot = insertBST(bstRoot, newCustomer); // Đưa thông tin vào Cây BST để lưu trữ tập trung

        // NGHIỆP VỤ HÀNG ĐỢI: Nếu rạp đã kín hết chỗ ngồi
        if (availableSeats == 0) {
            cout << "\n[THONG BAO] He thong da het ghe! Da tu dong dua khach hang '"
                << name << "' vao Danh sach cho uu tien.\n";
            waitingQueue.push(newCustomer); // Đẩy trực tiếp vào Hàng đợi ưu tiên, không cấp ghế
            return;
        }

        // NGHIỆP VỤ CẤP GHẾ TRÊN MẢNG 2 CHIỀU: Nếu rạp còn chỗ trống
        displaySeatMap();
        int r, c;
        while (true) {
            cout << "Chon hang (1-" << ROWS << "): "; cin >> r;
            cout << "Chon cot (1-" << COLS << "): "; cin >> c;
            r--; c--; // Trừ 1 đơn vị để chuyển đổi về chỉ số index (0 -> 4) của mảng C++

            // Kiểm tra ranh giới nhập dữ liệu có hợp lệ không
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
                if (seatMap[r][c] == "") { // Nếu ô trên mảng 2 chiều đang trống
                    seatMap[r][c] = id;    // Ghi nhận Mã ID khách hàng đè vào ô ghế đó
                    availableSeats--;      // Trừ đi một ghế trống của hệ thống
                    totalRevenue += TICKET_PRICE; // Tích lũy cộng doanh thu tiền vé

                    privateUpdateSeatInBST(bstRoot, id, r, c); // Cập nhật tọa độ ghế vào cây BST

                    // Tạo đối tượng Giao dịch và đẩy vào đỉnh STACK để lưu lại lịch sử
                    Transaction tx;
                    tx.type = "BOOK";
                    tx.customer = newCustomer;
                    tx.row = r;
                    tx.col = c;
                    txHistory.push(tx);

                    cout << "\n[THANH CONG] Dat ve thanh cong cho khach hang " << name << " tai ghe [" << r + 1 << "][" << c + 1 << "].\n";
                    break; // Thoát khỏi vòng lặp nhập ghế
                }
                else {
                    cout << "[LOI] Ghe nay da co nguoi dat. Vui long chon ghe khac!\n";
                }
            }
            else {
                cout << "[LOI] Vi tri ghe khong hop le!\n";
            }
        }
    }

    // CHỨC NĂNG 3: Hủy vé đặt gần nhất (Kết hợp cơ chế Undo của Stack và điều phối tự động từ Priority Queue)
    void cancelTicket() {
        // Nếu ngăn xếp lịch sử rỗng tức là chưa có bất kỳ giao dịch nào để hoàn tác
        if (txHistory.empty()) {
            cout << "\n[LOI] Lich su giao dich trong. Khong co ve nao de huy!\n";
            return;
        }

        // Lấy thông tin giao dịch mới nhất nằm ở đỉnh ngăn xếp STACK (Cơ chế LIFO)
        Transaction lastTx = txHistory.top();
        txHistory.pop(); // Xóa bỏ bản ghi đó ra khỏi Stack sau khi lấy dữ liệu

        int r = lastTx.row;
        int c = lastTx.col;
        string oldCustId = lastTx.customer.id;

        cout << "\n[HUY VE] Dang hoan tac giao dich gan nhat cua: " << lastTx.customer.name << "\n";

        // Giải phóng ô ghế vừa hủy trên mảng 2 chiều
        seatMap[r][c] = "";
        availableSeats++;              // Trả lại 1 ghế trống cho rạp
        totalRevenue -= TICKET_PRICE;  // Trừ bớt tiền ra khỏi tổng doanh thu hệ thống
        privateUpdateSeatInBST(bstRoot, oldCustId, -1, -1); // Đưa tọa độ ghế của khách bị hủy trên BST về -1

        cout << "-> Da huy ghe [" << r + 1 << "][" << c + 1 << "] thanh cong.\n";

        // TỰ ĐỘNG ĐIỀU PHỐI HÀNG CHỜ: Nếu phát hiện có người đang mòn mỏi chờ ở Priority Queue
        if (!waitingQueue.empty()) {
            // Bốc ngay người ở đầu hàng đợi ra (Người này chắc chắn có mức ưu tiên cao nhất tại thời điểm đó)
            Customer nextCust = waitingQueue.top();
            waitingQueue.pop(); // Xóa người này ra khỏi danh sách chờ

            // Ép cấp chỗ ngồi vừa trống cho người ưu tiên này trên mảng 2 chiều
            seatMap[r][c] = nextCust.id;
            availableSeats--; // Khấu trừ lại số ghế trống
            totalRevenue += TICKET_PRICE; // Thu tiền vé của khách mới
            privateUpdateSeatInBST(bstRoot, nextCust.id, r, c); // Cập nhật tọa độ ghế mới cho khách trên BST

            // Tạo bản ghi giao dịch mới đẩy ngược lại vào Stack để bảo toàn chuỗi logic hoàn tác tiếp theo
            Transaction newTx;
            newTx.type = "BOOK";
            newTx.customer = nextCust;
            newTx.row = r;
            newTx.col = c;
            txHistory.push(newTx);

            cout << "[TU DO DAT] Ghe trong [" << r + 1 << "][" << c + 1 << "] duoc tu dong cap cho nguoi cho: "
                << nextCust.name << " (Muc uu tien: " << nextCust.priority << ")\n";
        }
    }

    // CHỨC NĂNG 4: Tìm kiếm hồ sơ khách hàng bằng mã ID dựa trên cấu trúc cây BST
    void searchCustomer(string id) {
        BSTNode* result = searchBST(bstRoot, id); // Gọi hàm tìm kiếm đệ quy trên cây
        if (result) {
            cout << "\n=== THONG TIN KHACH HANG TIM THAY ===\n";
            cout << "Ma dinh danh (ID): " << result->customer.id << "\n";
            cout << "Ho ten: " << result->customer.name << "\n";
            string pType = (result->customer.priority == 3) ? "VIP" : ((result->customer.priority == 2) ? "Nguoi cao tuoi" : "Thuong");
            cout << "Doi tuong uu tien: " << pType << "\n";
            if (result->customer.bookedRow != -1) {
                cout << "Trang thai ve: Da dat - Ghe [" << result->customer.bookedRow + 1 << "][" << result->customer.bookedCol + 1 << "]\n";
            }
            else {
                cout << "Trang thai ve: Dang xep hang cho\n";
            }
        }
        else {
            cout << "\n[LOI] Khong tim thay khach hang co ma ID: " << id << " tren he thong!\n";
        }
    }

    // CHỨC NĂNG 5: Gọi lệnh duyệt cây In-order để xuất ra toàn bộ danh sách khách hàng
    void printAllCustomers() {
        cout << "\n====================== DANH SACH KHACH HANG TREN HE THONG (BST) ======================\n";
        cout << "--------------------------------------------------------------------------------------\n";
        if (!bstRoot) {
            cout << " He thong chua co khach hang nao dang ky.\n";
            return;
        }
        inOrderBST(bstRoot); // Kích hoạt hàm duyệt cây
        cout << "--------------------------------------------------------------------------------------\n";
    }

    // CHỨC NĂNG 6: Xuất các chỉ số quản trị hệ thống
    void showStatistics() {
        cout << "\n========= BAO CAO THONG KE HE THONG =========\n";
        cout << " Tong so ghe trong hien tai : " << availableSeats << " ghe.\n";
        cout << " Tong so ghe da co nguoi dat: " << (ROWS * COLS) - availableSeats << " ghe.\n";
        cout << " So nguoi dang cho trong hang doi: " << waitingQueue.size() << " nguoi.\n";
        cout << " Tong doanh thu hien tai    : " << totalRevenue << " VND\n";
        cout << "==============================================\n";
    }
};

// ==========================================
// 3. GIAO DIỆN ĐIỀU KHIỂN CHƯƠNG TRÌNH (MAIN)
// ==========================================
int main() {
    TicketSystem system;
    int choice;

    // Giả lập nạp sẵn 3 khách hàng mẫu ban đầu vào bộ nhớ hệ thống để test chức năng dễ dàng
    system.bookTicket("KH003", "Nguyen Van A", 1);
    system.bookTicket("KH001", "Tran Thi VIP", 3);
    system.bookTicket("KH002", "Bac Tam Cao Tuoi", 2);

    // Vòng lặp hiển thị Menu CLI tương tác vô hạn cho đến khi bấm thoát
    while (true) {
        cout << "\n================= MENU HE THONG DAT VE =================";
        cout << "\n1. Hien thi so do ghe ngoi (ASCII)";
        cout << "\n2. Dat ve moi";
        cout << "\n3. Huy ve dat gan nhat (Hoan tac & Cap tu dong cho hang doi)";
        cout << "\n4. Tim kiem thong tin khach hang theo ma ID (BST)";
        cout << "\n5. Xuat toan bo danh sach khach hang";
        cout << "\n6. Xem thong ke (Ghe trong, Doanh thu)";
        cout << "\n7. Thoat chuong trang";
        cout << "\nLua chon cua ban (1-7): ";

        // Vòng lặp bảo vệ hệ thống: Bẫy lỗi nếu người dùng cố tình nhập chữ thay vì nhập số
        while (!(cin >> choice)) {
            cout << "[LOI] Vui long nhap dung ky tu so: ";
            cin.clear();            // Xóa trạng thái lỗi của dòng nhập cin
            cin.ignore(123, '\n');  // Xóa bỏ các ký tự rác đang tồn đọng trong bộ đệm nhập
        }

        // Điều kiện thoát chương trình lập tức
        if (choice == 7) {
            cout << "\nCamon ban da su dung he thong!\n";
            break;
        }

        string id, name;
        int priority;

        // Định tuyến chức năng dựa trên số người dùng vừa chọn
        switch (choice) {
        case 1:
            system.displaySeatMap(); // Gọi chức năng in sơ đồ
            break;
        case 2:
            cout << "\n--- DANG KY DAT VE ---\n";
            cout << "Nhap ma khach hang (VD: KH005): "; cin >> id;
            cin.ignore(); // Xóa ký tự xuống dòng '\n' còn sót lại trong bộ đệm trước khi dùng getline
            cout << "Nhap ho ten khach hang (Khong dau): "; getline(cin, name);
            cout << "Chon muc do uu tien (3: VIP, 2: Nguoi cao tuoi, 1: Thuong): "; cin >> priority;
            if (priority < 1 || priority > 3) priority = 1; // Nếu nhập sai dải số, ép về đối tượng Thường

            system.bookTicket(id, name, priority); // Gọi hàm đăng ký đặt chỗ
            break;
        case 3:
            system.cancelTicket(); // Gọi chức năng hoàn tác giao dịch
            break;
        case 4:
            cout << "\nNhap ma khach hang can tim: "; cin >> id;
            system.searchCustomer(id); // Gọi chức năng tìm kiếm thông tin khách hàng
            break;
        case 5:
            system.printAllCustomers(); // Gọi chức năng in danh sách dạng bảng cây nhị phân BST
            break;
        case 6:
            system.showStatistics(); // Gọi chức năng báo cáo doanh thu và ghế trống
            break;
        default:
            cout << "\n[LOI] Lua chon khong hop le! Vui long nhap lai tu 1 den 7.\n";
        }
    }
    return 0;
}