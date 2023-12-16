#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int main()
{
	//Đọc ảnh lena
	Mat A = imread("C:\\Users\\Admin\\Downloads\\lena.jpg");
	// Clone ảnh A sang ảnh B
	Mat B = A.clone();
	//  Tạo ra một ma trận kernel 3x3 chứa toàn số 1 và sau đó chia mỗi phần tử cho 9 để tạo ra một ma trận làm trọng số cho việc làm mờ.
	Mat kernel = Mat::ones(Size(3, 3), CV_32F) / (float)(9);
	// Hàm blur được sử dụng để làm mờ ảnh A và kết quả được lưu vào ảnh B. Kích thước của kernel là 3x3, Point(-1, -1) chỉ định điểm trung tâm của kernel, và tham số cuối cùng (4) chỉ định độ mờ.
	blur(A, B, Size(3, 3), Point(-1, -1), 1);
	if (!A.data)  //Kiểm tra xem đã lấy được ảnh chưa?
	{
		cout << "Khong tim thay anh!";
	}
	imshow("DOC ANH GOC", A);
	imshow("DOC ANH DICH", B);
	waitKey();
}
