#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

bool Check = false;

tuple<Mat, double, double> automatic_brightness_and_contrast(Mat image, double clip_hist_percent = 1.0) {
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);

	// Tính toán histogram của ảnh xám
	Mat hist;
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	calcHist(&gray, 1, nullptr, Mat(), hist, 1, &histSize, &histRange);

	// Tính toán phân phối tích lũy của histogram
	vector<float> accumulator;
	accumulator.push_back(hist.at<float>(0));
	for (int index = 1; index < histSize; ++index) {
		accumulator.push_back(accumulator[index - 1] + hist.at<float>(index));
	}

	// Xác định điểm cắt trái và phải dựa trên phân phối tích lũy
	float maximum = accumulator.back();
	clip_hist_percent *= (maximum / 100.0);
	clip_hist_percent /= 2.0;

	// Điểm cắt phải
	int minimum_gray = 0;
	while (accumulator[minimum_gray] < clip_hist_percent) {
		++minimum_gray;
	}

	// Tính toán giá trị alpha và beta để điều chỉnh độ sáng và độ tương phản
	int maximum_gray = histSize - 1;
	while (accumulator[maximum_gray] >= (maximum - clip_hist_percent)) {
		--maximum_gray;
	}

	// Tính toán giá trị alpha và beta
	double alpha = 255.0 / (maximum_gray - minimum_gray);
	double beta = -minimum_gray * alpha;

	//Trả về kết quả và thông số alpha, beta
	Mat auto_result;
	convertScaleAbs(image, auto_result, alpha, beta);
	return make_tuple(auto_result, alpha, beta);
}

// Khử nhiễu ảnh bằng bộ lọc Blur
void Bai4() {
	//Đọc ảnh lena
	Mat A = imread("C:\\Users\\Admin\\Downloads\\lena.jpg");
	// Clone ảnh A sang ảnh B
	Mat B = A.clone();
	//  Tạo ra một ma trận kernel 3x3 chứa toàn số 1 và sau đó chia mỗi phần tử cho 9 để tạo ra một ma trận làm trọng số cho việc làm mờ.
	Mat kernel = Mat::ones(Size(3, 3), CV_32F) / (float)(9);
	// Hàm blur được sử dụng để làm mờ ảnh A và kết quả được lưu vào ảnh B. Kích thước của kernel là 3x3,
	// Point(-1, -1) chỉ định điểm trung tâm của kernel, và tham số cuối cùng (4) chỉ định độ mờ.
	blur(A, B, Size(3, 3), Point(-1, -1), 1);
	//Kiểm tra xem đã lấy được ảnh chưa? nếu chưa thì console "khong tim thay anh"
	if (!A.data)
	{
		cout << "Khong tim thay anh!";
	}
	// In ra ảnh gốc
	imshow("DOC ANH GOC", A);
	// In ra ảnh đích
	imshow("DOC ANH DICH", B);
	waitKey();
}

// Phát hiện đường thẳng, đường tròn
void Bai5() {
	Mat src = imread("C:\\Users\\Admin\\Downloads\\review.png");
	Mat image;
	resize(src, image, cv::Size(), 0.75, 0.75);
	Mat grayImage;
	// Chuyển ảnh màu sang ảnh xám
	cvtColor(image, grayImage, COLOR_BGR2GRAY);
	// Làm mờ ảnh
	GaussianBlur(grayImage, grayImage, Size(9, 9), 2, 2);

	// Lọc biên ảnh bằng bộ lọc Canny
	Mat edges;
	Canny(grayImage, edges, 50, 150);

	// Sử dụng vecto 4 chiều
	vector<Vec4i> lines;
	// Sử dụng hàm HoughLinesP để phát hiện đường thẳng
	HoughLinesP(edges, lines, 1, CV_PI / 180, 50, 60, 10);
	
	// Sử dụng vecto 3 chiều
	vector<Vec3f> circles;
	// Sử dụng hàm HoughCircles để phát hiện đường tròn
	HoughCircles(grayImage, circles, HOUGH_GRADIENT, 1, 100, 200, 100, 0, 0);

	//Vẽ đường thẳng trên ảnh gốc
	for (int i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 2);
	}
	//Vẽ đường tròn trên ảnh gốc
	for (int i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1])); // vẽ tâm vòng
		int radius = cvRound(circles[i][2]); // Bán kính
		circle(image, center, radius, Scalar(0, 0, 255), 2, 8, 0); // Vẽ đƣờng viền vòng
	}
	// Hiện ảnh lên màn hình
	imshow("Gray Image", grayImage);
	imshow("Edges", edges);
	imshow("Detected Lines", image);
	waitKey(0);
}

// Chỉnh sáng tự động trong văn bản scan
void Bai6() {
	// Đọc ảnh văn bản tối
	Mat image = imread("C:\\Users\\Admin\\Downloads\\vanban.jpg");
	Mat image_resize;
	resize(image, image_resize, Size(), 0.75, 0.75);

	if (image.empty()) {
		cout << "Error: Couldn't read the image." << endl;
	}
	else {
		Mat auto_result;
		double alpha, beta;
		// gán giá trị trả về từ hàm automatic_brightness_and_contrast vào các biến auto_result, alpha, và beta.
		tie(auto_result, alpha, beta) = automatic_brightness_and_contrast(image_resize);

		cout << "alpha: " << alpha << endl;
		cout << "beta: " << beta << endl;

		imshow("goc", image_resize);
		imshow("result.jpg", auto_result);
		waitKey(0);
	}
}
// Tìm contour trong ảnh
void Bai7() {
	Mat image = imread("C:\\Users\\Admin\\Downloads\\review.png");				//Đọc ảnh
	Mat image_resize;
	Mat image2 = image_resize.clone();
	resize(image, image_resize, Size(), 0.75, 0.75);							//Resize kích thước
	Mat grayImg;
	cvtColor(image_resize, grayImg, COLOR_BGR2GRAY);							//Chuyển sang màu xám
	Mat binImg;
	threshold(grayImg, binImg, 100, 255, THRESH_BINARY);						// Chuyển sang ảnh nhị phân
	vector<vector<Point>> contours;
	findContours(binImg, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);				//Tìm viền trong ảnh nhị phân, trong đó:
																				//binImg: ảnh nhị phân
																				//coutours: số coutours đã phát hiện
																				//RETR_TREE: trích xuất đường viền bằng cây đặc trưng
																				//CHAIN_APPROX_SIMPLE: phương pháp xấp xỉ đường viền
	cout << "Contours number found: " << contours.size() << std::endl;
	for (int iContours = 0; iContours < contours.size(); iContours++)			// Đếm từng điểm ảnh trong từng viền
	{
		cout << "Contour #" << iContours + 1 << ": { ";
		for (Point p : contours[iContours]) {
			cout << "( " << p.x << ", " << p.y << " ) ";
		}
		cout << "}\n";
	}

	drawContours(image_resize, contours, -1, Scalar(0, 255, 0), 2);				//vẽ contours trên ảnh, trong đó:
																				//image_resize: ảnh gốc
																				//coutours: số contours đã phát hiện
																				//-1: chỉ số đường viền
																				//Scalar(0,255,0): màu đường viền
																				//1: độ dày đường viền

	imshow("gray", grayImg);
	imshow("binImg", binImg);
	imshow("drawContour", image_resize);
	waitKey();
}

void Action() {		
	cout << "\t\t\tKHO CODE THAI HOC XIN KINH CHAO QUY KHACH!!!\n\n";
	cout << "\t\t\tVUI LONG XEM VA CHON MUC DE THUC THI CHUONG TRINH:\n" << endl;
	cout << "\t1.IN RA CAC MAT HANG TRONG KHO HIEN TAI.\n";
	cout << "\t2.NHAP DU LIEU CAC MAT HANG NHAP KHO.\n";
	cout << "\t3.XUAT RA PHIEU NHAP DON HANG.\n";
	cout << "\t4.KHU NHIEU BANG BO LOC BLUR \n";
	cout << "\t5.PHAT HIEN DUONG THANG, DUONG TRON \n";
	cout << "\t6.CHINH SANG VAN BAN TU DONG\n";
	cout << "\t7.TIM VA VE CONTOUR TRONG ANH \n";
	cout << "SO BAN CHON LA: "; int i; cin >> i;
	switch (i) {
	case 1:
	{
		break;
	}
	case 2:
	{
		break;
	}
	case 3:
	{
		break;
	}
	case 4:
	{
		Bai4();
		break;
	}

	case 5:
	{
		Bai5();
		break;
	}
	case 6:
	{
		Bai6();
		break;
	}
	case 7:
	{
		Bai7();
		break;
	}

	default:
		cout << "Invalid input." << endl;
		system("pause");
		break;
	}
}

int main()
{
	Action();
	while (true) {
		system("cls");
		cout << "BAN CO MUON TIEP TUC CHUONG TRINH KHONG ?(1 - CO | 2 - KHONG)   "; string i; getline(cin, i);
		cout << endl;
		if (i == "1") {
			system("cls");
			Check = false;
			Action();
		}
		else if (i == "2") {
			cout << "CAM ON BAN DA SU DUNG DICH VU CUA CHUNG TOI.";
			break;
		}
	}
}
