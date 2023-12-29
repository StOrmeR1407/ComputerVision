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
	resize(src, image, Size(), 0.75, 0.75);
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

void Bai8() {
	vector<Rect> boundRects;
	Mat image = imread("C:\\Users\\Admin\\Downloads\\vanban2.jpg");				//Đọc ảnh
	Mat image_resize;
	resize(image, image_resize, Size(), 0.75, 0.75);							//Resize kích thước
	Mat grayImg;
	cvtColor(image_resize, grayImg, COLOR_BGR2GRAY);							//Chuyển sang màu xám
	Mat sobImg;
	Sobel(grayImg, sobImg, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Mat binImg;
	threshold(sobImg, binImg, 0, 255, THRESH_OTSU + THRESH_BINARY);
	Mat matElement;
	matElement = getStructuringElement(MORPH_RECT, Size(19, 3));
	morphologyEx(binImg, binImg, MORPH_CLOSE , matElement); 
	vector<vector<Point> > contours;
	findContours(binImg, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	vector<vector<Point> > contours_poly(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > 100)
		{
			approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
			Rect appRect(boundingRect(Mat(contours_poly[i])));
			if (appRect.width > appRect.height)
			{
				appRect.x -= image_resize.cols / 100;
				appRect.width += image_resize.cols / 100;
				boundRects.push_back(appRect);
			}
		}
	}
	for (const Rect& rect : boundRects)
	{
		rectangle(image_resize, rect, Scalar(0, 0, 255), 2); // Vẽ đỏ với độ dày 2
	}
	imshow("Goc", image);
	imshow("Sobel", sobImg);
	imshow("Binary", binImg);
	imshow("Find Text", image_resize);
	waitKey();
}

void Bai9() {
	Mat src = imread("E:\\Template\\dongxu1.jfif");

	Mat pre = src.clone();
	int b = pre.at<cv::Vec3b>(5, 5)[0];
	int g = pre.at<cv::Vec3b>(5, 5)[1];
	int r = pre.at<cv::Vec3b>(5, 5)[2];

	Mat mask;
	inRange(pre, Scalar(b - 20, g - 25, r - 25), Scalar(b + 8, g + 8, r + 8), mask);
	pre.setTo(Scalar(0, 0, 0), mask);
	pre.setTo(Scalar(255, 255, 255), ~mask);
	imshow("blackbg", pre);
	blur(pre, pre, Size(3, 3), Point(-1, -1), 4);
	imshow("blur", pre);
	Mat oldbw;
	Mat bw;
	cvtColor(pre, oldbw, COLOR_BGR2GRAY);
	threshold(oldbw, bw, 90, 255, THRESH_BINARY);
	imshow("binImg", bw);
	Mat dist;
	distanceTransform(bw, dist, DIST_L2, 3);
	imshow("dist1", dist);
	normalize(dist, dist, 0, 1.1, NORM_MINMAX);
	imshow("dist2", dist);
	threshold(dist, dist, .5, 1., THRESH_BINARY);
	imshow("dist3", dist);
	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);
	vector<vector<Point> > contours;
	findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	int ncomp = contours.size();

	cout <<" Number of coins: " << ncomp << endl;

	// Create the marker image for the watershed algorithm
	Mat markers = Mat::zeros(dist.size(), CV_32SC1);

	// Draw the foreground markers
	for (int i = 0; i < ncomp; i++)
		drawContours(markers, contours, i, Scalar::all(i + 1), -1);

	// Draw the background marker
	circle(markers, Point(5, 5), 3, CV_RGB(255, 255, 255), -1);

	// Perform the watershed algorithm
	watershed(src, markers);

	// Generate random colors
	vector<Vec3b> colors;
	for (int i = 0; i < ncomp; i++)
	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);

		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	// Create the result image
	Mat dst = Mat::zeros(markers.size(), CV_8UC3);

	// Fill labeled objects with random colors
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index > 0 && index <= ncomp)
				dst.at<Vec3b>(i, j) = colors[index - 1];
			else
				dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
		}
	}
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);

}

void Test() {
	cv::Mat image = cv::imread("E:\\Template\\dongxu2.jfif");

	int b = image.at<cv::Vec3b>(5, 5)[0];
	int g = image.at<cv::Vec3b>(5, 5)[1];
	int r = image.at<cv::Vec3b>(5, 5)[2];

	Mat mask;
	inRange(image, Scalar(b - 20, g - 25, r - 25), Scalar(b + 8, g + 8, r + 8), mask);
	image.setTo(Scalar(0, 0, 0), mask);
	image.setTo(Scalar(255, 255, 255), ~mask);
	blur(image, image, Size(3, 3), Point(-1, -1), 4);
	imshow("Original Image", image);
	cv::waitKey(0);
}

std::vector<std::vector<cv::Point>> ClampContourBySize(std::vector<std::vector<cv::Point>> contours, int minValue, int maxValue)
{
	std::vector<std::vector<cv::Point>> result;
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::Rect r = cv::boundingRect(contours[i]);
		float distance = abs(r.width - r.height) / (r.width + r.height);
		if (distance < 0.5)
		{
			if (minValue <= r.width && r.width <= maxValue && minValue <= r.height && r.height <= maxValue)
			{
				result.push_back(contours[i]);
			}
		}

	}
	return result;
}

void Bai10() {
	cv::Mat image = cv::imread("E:\\Template\\tomgiong3.jfif");
	Mat matGray;
	cvtColor(image, matGray, COLOR_BGR2GRAY);
	cv::imshow("mat gray", matGray);

	//blur
	cv::Mat matBlur;
	cv::blur(matGray, matBlur, cv::Size(29, 29));

	//threshold
	cv::Mat matBinary;
	cv::adaptiveThreshold(matBlur, matBinary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 21, 0);
	cv::imshow("binary", matBinary);


	//Finding sure foreground area
	cv::Mat dist_transform;
	cv::distanceTransform(matBinary, dist_transform, DIST_L2, 3);


	cv::Mat matNorm;
	cv::normalize(dist_transform, matNorm, 0, 1, cv::NORM_MINMAX);
	cv::imshow("matNorm", matNorm);


	double min, max;
	cv::minMaxLoc(dist_transform, &min, &max);
	cv::Mat sure_fg;
	cv::threshold(dist_transform, sure_fg, 0.4 * max, 255, THRESH_BINARY);
	cv::imshow("sure_fg", sure_fg);


	//convert to 1 channel
	sure_fg.convertTo(sure_fg, CV_8U);
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(sure_fg, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	contours = ClampContourBySize(contours, 5, 20);

	for (int i = 0; i < contours.size(); i++)
	{
		cv::RotatedRect rrect = cv::minAreaRect(contours[i]);
		cv::rectangle(image, rrect.boundingRect(), Scalar(0, 255, 0), 1);
	}

	cv::imshow("Output", image);

	cout << contours.size();
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
	cout << "\t8.TIM VI TRI TEXT TRONG ANH \n";
	cout << "\t9.DEM DONG XU NAM CHONG \n";
	cout << "\t10. DEM TOM \n";
	cout << "\t0.TEST PROGRAM \n";
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

	case 8:
	{
		Bai8();
		break;
	}

	case 9:
	{
		Bai9();
		break;
	}

	case 10:
	{
		Bai10();
		break;
	}

	case 0: {
		Test();
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
